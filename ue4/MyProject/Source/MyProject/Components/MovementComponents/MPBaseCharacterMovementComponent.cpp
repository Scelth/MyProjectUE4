#include "MPBaseCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "MyProject/Characters/PlayerCharacter.h"
#include "MyProject/Actors/Interactive/Environment/Ladder.h"
#include "MyProject/Actors/Interactive/Environment/Zipline.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/MPTypes.h"

#pragma region Base
void UMPBaseCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<APlayerCharacter>(), TEXT("ULegdeDetectorComponent::BeginPlay() only character can use ULegdeDetectorComponent"));
	CachedPlayerCharacter = StaticCast<APlayerCharacter*>(GetOwner());

	GetBaseCharacterOwner()->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &UMPBaseCharacterMovementComponent::OnPlayerCapsuleHit);
	GetBaseCharacterOwner()->GetCharacterMovement()->SetPlaneConstraintEnabled(true);
}

float UMPBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();

	if (bIsSprinting)
	{
		Result = SprintSpeed;
	}

	if (bIsProning)
	{
		Result = ProneSpeed;
	}

	if (bIsOutOfStamina)
	{
		Result = ExhaustedSpeed;
	}

	if (CachedPlayerCharacter->IsAiming())
	{
		Result = CachedPlayerCharacter->GetAimingMovementSpeed();
	}

	if (IsOnLadder())
	{
		Result = ClimbingMaxSpeed;
	}

	if (IsOnWall())
	{
		Result = MaxWallRunSpeed;
	}

	if (IsOnZipline())
	{
		Result = MaxZiplineSpeed;
	}

	return Result;
}

void UMPBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (MovementMode == MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfHeight);
	}

	else if (PreviousMovementMode == MOVE_Swimming)
	{
		ACharacter* DefaultCharacter = CachedPlayerCharacter->GetClass()->GetDefaultObject<ACharacter>();

		float DefaultRadius = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		float DefaultHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultRadius, DefaultHalfHeight);
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		CurrentLadder = nullptr;
	}		
	
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		CurrentZipline = nullptr;
	}	

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
			case (uint8)ECustomMovementMode::CMOVE_Mantling:
			{
				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UMPBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
				break;
			}

			default:
			{
				break;
			}
		}
	}
}
#pragma endregion

#pragma region Phys
void UMPBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (bIsForceRotation)
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		// Accumulate a desired new rotation.
		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
		}

		else
		{
			ForceTargetRotation = FRotator::ZeroRotator;
			bIsForceRotation = false;
		}

		return;
	}

	if (IsOnLadder())
	{
		return;
	}

	if (IsOnWall())
	{
		return;
	}

	Super::PhysicsRotation(DeltaTime);
}

void UMPBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
		case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{
			PhysMantling(DeltaTime, Iterations);
			break;
		}

		case (uint8)ECustomMovementMode::CMOVE_Ladder:
		{
			PhysLadder(DeltaTime, Iterations);
			break;
		}

		case (uint8)ECustomMovementMode::CMOVE_WallRun:
		{
			PhysWallRun(DeltaTime, Iterations);
			break;
		}		
		
		case (uint8)ECustomMovementMode::CMOVE_Zipline:
		{
			PhysZipline(DeltaTime, Iterations);
			break;
		}

		default:
		{
			break;
		}
	}

	Super::PhysCustom(DeltaTime, Iterations);
}

void UMPBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;

	FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);

	float PositionAlpha = MantlingCurveValue.X;
	float XYCorrectionAlpha = MantlingCurveValue.Y;
	float ZCorrectionAlpha = MantlingCurveValue.Z;

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);

	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha);
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);

	FVector Delta = NewLocation - GetActorLocation();

	FHitResult Hit;

	SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UMPBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 1.f, false, ClimbingOnLadderBrakingDeceleration);

	FVector Delta = Velocity * DeltaTime;

	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	GetOwner()->SetActorRotation(TargetOrientationRotation);

	if (HasAnimRootMotion())
	{
		FHitResult Hit;

		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
	}

	FVector NewPos = GetActorLocation() + Delta;

	float NewPosProjection = GetActorToCurrentLadderProjection(NewPos);

	if (NewPosProjection < MinLadderBottomOffset)
	{
		DetachFromLadder(EDetachFromInteractionMethod::ReachingTheBottom);
		return;
	}

	else if (NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromLadder(EDetachFromInteractionMethod::ReachingTheTop);
		return;
	}

	FHitResult Hit;

	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

void UMPBaseCharacterMovementComponent::PhysWallRun(float DeltaTime, int32 iterations)
{
	WallRunTimeElapsed += DeltaTime;

	if (WallRunTimeElapsed >= MaxWallRunTime)
	{
		DetachFromWall();
		return;
	}

	FVector Delta = Velocity * DeltaTime;
	FHitResult HitResult;

	FVector LineTraceDirection = CurrentWallRunSide == EWallRunSide::Right ? GetOwner()->GetActorRightVector() : -GetOwner()->GetActorRightVector();
	float LineTraceLength = 200.0f;

	FVector StartPosition = GetActorLocation();
	FVector EndPosition = StartPosition + LineTraceLength * LineTraceDirection;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetBaseCharacterOwner());

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPosition, EndPosition, ECC_WallRunnable, QueryParams))
	{
		EWallRunSide Side = EWallRunSide::None;
		FVector Direction = FVector::ZeroVector;
		GetWallRunSideAndDirection(HitResult.ImpactNormal, Side, Direction);

		if (Side != CurrentWallRunSide)
		{
			DetachFromWall();
		}

		else
		{
			CurrentWallRunDirection = Direction;
			Velocity = GetMaxSpeed() * CurrentWallRunDirection;
			Delta = Velocity * DeltaTime;
		}

		FRotator TargetOrientationRotation = Direction.ToOrientationRotator();
		GetOwner()->SetActorRotation(TargetOrientationRotation);
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, HitResult);
	}

	else
	{
		DetachFromWall();
		return;
	}
}

void UMPBaseCharacterMovementComponent::PhysZipline(float DeltaTime, int32 iterations)
{
	FRotator TargetOrientationRotation = CurrentZipline->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw = 0.f;
	GetOwner()->SetActorRotation(TargetOrientationRotation);

	FVector StartPoint = CurrentZipline->GetUpperPillarMeshComponent()->GetComponentLocation();
	FVector EndPoint = CurrentZipline->GetLowerPillarMeshComponent()->GetComponentLocation();
	FVector Direction = (EndPoint - StartPoint).GetSafeNormal();

	FVector Delta = Direction * GetMaxSpeed() * DeltaTime;

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);

	const float DetachRadius = 100.f;

	if (FVector::Dist(GetOwner()->GetActorLocation(), CurrentZipline->GetLowerPillarMeshComponent()->GetComponentLocation()) <= DetachRadius)
	{
		DetachFromZipline();
	}
}
#pragma endregion

#pragma region Sprint
void UMPBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = true;
}

void UMPBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = false;
}
#pragma endregion

#pragma region Prone
void UMPBaseCharacterMovementComponent::StartProne()
{
	CachedPlayerCharacter->GetCapsuleComponent()->SetCapsuleSize(ProneCapsuleRadius, ProneCapsuleHalfHeight);

	HeightAdjust = CachedPlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	CachedPlayerCharacter->OnStartProne(HeightAdjust);

	bIsProning = true;
}

void UMPBaseCharacterMovementComponent::StopProne(bool bIsFromCrouch)
{
	ACharacter* DefaultCharacter = CachedPlayerCharacter->GetClass()->GetDefaultObject<ACharacter>();

	if (bIsFromCrouch)
	{
		UCharacterMovementComponent* MovementComponent = DefaultCharacter->GetCharacterMovement();
		CachedPlayerCharacter->GetCapsuleComponent()->SetCapsuleSize(MovementComponent->CrouchedHalfHeight * ScaleCrouchHalfHeight,
			MovementComponent->CrouchedHalfHeight);
	}

	else
	{
		float TargetRadius = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		float TargetHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

		float CurrentRadius = CachedPlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		float CurrentHeight = CachedPlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

		float NewRadius = FMath::FInterpTo(CurrentRadius, TargetRadius, GetWorld()->DeltaTimeSeconds, InterpSpeed);
		float NewHeight = FMath::FInterpTo(CurrentHeight, TargetHeight, GetWorld()->DeltaTimeSeconds, InterpSpeed);

		CachedPlayerCharacter->GetCapsuleComponent()->SetCapsuleSize(NewRadius, NewHeight);
	}

	HeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() -
		CachedPlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	CachedPlayerCharacter->OnEndProne(HeightAdjust);

	bIsProning = false;
}
#pragma endregion

#pragma region Mantle
bool UMPBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling || bIsMantling;
}

void UMPBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	if (CachedPlayerCharacter.IsValid() && CachedPlayerCharacter->bIsCrouched)
	{
		CachedPlayerCharacter->UnCrouch();
	}

	CurrentMantlingParameters = MantlingParameters;

	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);

	bIsMantling = true;
}

void UMPBaseCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
	bIsMantling = false;
}
#pragma endregion

#pragma region Ladder
void UMPBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	CurrentLadder = Ladder;

	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.f;

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();

	float Projection = GetActorToCurrentLadderProjection(GetActorLocation());

	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + Projection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;

	if (CurrentLadder->IsOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();
	}

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Ladder);
}

float UMPBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();

	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

void UMPBaseCharacterMovementComponent::DetachFromLadder(EDetachFromInteractionMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
		case EDetachFromInteractionMethod::ReachingTheTop:
		{
			GetBaseCharacterOwner()->Mantle(true);
			break;
		}

		case EDetachFromInteractionMethod::ReachingTheBottom:
		{
			SetMovementMode(MOVE_Walking);
			break;
		}

		case EDetachFromInteractionMethod::JumpOff:
		{
			FVector JumpDirection = CurrentLadder->GetActorForwardVector();
			FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
			
			SetMovementMode(MOVE_Falling);

			ForceTargetRotation = JumpDirection.ToOrientationRotator();

			bIsForceRotation = true;

			Launch(JumpVelocity);
			break;
		}

		case EDetachFromInteractionMethod::Fall:
		default:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
	}

	SetMovementMode(MOVE_Falling);
}

bool UMPBaseCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Ladder;
}

float UMPBaseCharacterMovementComponent::GetLadderSpeedRation() const
{
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingMaxSpeed;
}
#pragma endregion

#pragma region WallForRun
void UMPBaseCharacterMovementComponent::AttachToWall(EWallRunSide Side, const FVector& Direction)
{
	CurrentWallRunSide = Side;
	CurrentWallRunDirection = Direction;

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_WallRun);
}

void UMPBaseCharacterMovementComponent::DetachFromWall(EDetachFromInteractionMethod DetachFromWallMethod)
{
	WallRunTimeElapsed = 0.f;

	switch (DetachFromWallMethod)
	{
		case EDetachFromInteractionMethod::JumpOff:
		{
			FVector JumpDirection = FVector::ZeroVector;

			if (CurrentWallRunSide == EWallRunSide::Right)
			{
				JumpDirection = FVector::CrossProduct(CurrentWallRunDirection, FVector::UpVector).GetSafeNormal();
			}

			else
			{
				JumpDirection = FVector::CrossProduct(FVector::UpVector, CurrentWallRunDirection).GetSafeNormal();
			}

			JumpDirection += FVector::UpVector;

			GetBaseCharacterOwner()->LaunchCharacter(JumpZVelocity * JumpDirection.GetSafeNormal(), false, true);
			break;
		}

		case EDetachFromInteractionMethod::Fall:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
	}

	SetMovementMode(MOVE_Falling);
}

void UMPBaseCharacterMovementComponent::GetWallRunSideAndDirection(const FVector& HitNormal, EWallRunSide& OutSide, FVector& OutDirection) const
{
	if (FVector::DotProduct(HitNormal, GetOwner()->GetActorRightVector()) > 0)
	{
		OutSide = EWallRunSide::Left;
		OutDirection = FVector::CrossProduct(HitNormal, FVector::UpVector).GetSafeNormal();
	}

	else
	{
		OutSide = EWallRunSide::Right;
		OutDirection = FVector::CrossProduct(FVector::UpVector, HitNormal).GetSafeNormal();
	}
}

bool UMPBaseCharacterMovementComponent::IsOnWall() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_WallRun ;
}


void UMPBaseCharacterMovementComponent::OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || IsOnWall())
	{
		return;
	}

	if (!OtherComp->GetCollisionResponseToChannel(ECC_WallRunnable))
	{
		return;
	}

	if (!IsFalling())
	{
		return;
	}

	EWallRunSide Side = EWallRunSide::None;
	FVector Direction = FVector::ZeroVector;
	FVector HitNormal = Hit.ImpactNormal;

	GetWallRunSideAndDirection(HitNormal, Side, Direction);

	AttachToWall(Side, Direction);
}
#pragma endregion

#pragma region Zipline
void UMPBaseCharacterMovementComponent::AttachToZipline(const AZipline* Zipline)
{
	CurrentZipline = Zipline;

	FVector StartPoint = CurrentZipline->GetUpperPillarMeshComponent()->GetComponentLocation();
	FVector ClosestPoint = FMath::ClosestPointOnSegment(GetOwner()->GetActorLocation(), StartPoint, CurrentZipline->GetLowerPillarMeshComponent()->GetComponentLocation());

	float CapsuleHeight = GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector AdjustedPosition = ClosestPoint - (GetOwner()->GetActorUpVector() * CapsuleHeight);

	GetOwner()->SetActorLocation(AdjustedPosition);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Zipline);
}

void UMPBaseCharacterMovementComponent::DetachFromZipline()
{
	SetMovementMode(MOVE_Falling);
}

bool UMPBaseCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Zipline;
}
#pragma endregion

AMPBaseCharacter* UMPBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return StaticCast<AMPBaseCharacter*>(CharacterOwner);
}