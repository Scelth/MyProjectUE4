#include "MPBaseCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "MyProject/Characters/PlayerCharacter.h"
#include "MyProject/Actors/Interactive/Environment/Ladder.h"
#include "MyProject/Characters/MPBaseCharacter.h"

#pragma region Base
void UMPBaseCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<APlayerCharacter>(), TEXT("ULegdeDetectorComponent::BeginPlay() only character can use ULegdeDetectorComponent"));
	CachedPlayerCharacter = StaticCast<APlayerCharacter*>(GetOwner());
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

	if (HasAnimRootMotion())
	{
		FHitResult Hit;

		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
	}

	FVector NewPos = GetActorLocation() + Delta;

	float NewPosProjection = GetActorToCurrentLadderProjection(NewPos);

	if (NewPosProjection < MinLadderBottomOffset)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
		return;
	}

	else if (NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
		return;
	}

	FHitResult Hit;

	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
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

void UMPBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
		case EDetachFromLadderMethod::ReachingTheTop:
		{
			GetBaseCharacterOwner()->Mantle(true);
			break;
		}

		case EDetachFromLadderMethod::ReachingTheBottom:
		{
			SetMovementMode(MOVE_Walking);
			break;
		}

		case EDetachFromLadderMethod::JumpOff:
		{
			FVector JumpDirection = CurrentLadder->GetActorForwardVector();
			FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
			
			SetMovementMode(MOVE_Falling);

			ForceTargetRotation = JumpDirection.ToOrientationRotator();

			bIsForceRotation = true;

			Launch(JumpVelocity);
			break;
		}

		case EDetachFromLadderMethod::Fall:
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

AMPBaseCharacter* UMPBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return StaticCast<AMPBaseCharacter*>(CharacterOwner);
}