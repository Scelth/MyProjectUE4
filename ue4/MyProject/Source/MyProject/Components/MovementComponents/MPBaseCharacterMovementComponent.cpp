#include "MPBaseCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "MyProject/Characters/PlayerCharacter.h"
#include "Curves/CurveVector.h"

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

	return Result;
}

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

void UMPBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::CMOVE_Mantling:
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
		break;
	}

	default:
		break;
	}

	Super::PhysCustom(DeltaTime, Iterations);
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