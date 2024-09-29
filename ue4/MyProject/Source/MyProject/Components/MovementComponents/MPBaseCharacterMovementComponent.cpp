// Fill out your copyright notice in the Description page of Project Settings.


#include "MPBaseCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
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

	return Result;
}

void UMPBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = true; //1
}

void UMPBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = false; //0
}

void UMPBaseCharacterMovementComponent::StartProne()
{
	CachedPlayerCharacter->GetCapsuleComponent()->SetCapsuleSize(ProneCapsuleRadius, ProneCapsuleHalfHeight);

	HeightDifference = CachedPlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ProneCapsuleHalfHeight;

	CachedPlayerCharacter->OnStartProne();

	bIsProning = true;
}

void UMPBaseCharacterMovementComponent::StopProne(bool FromCrouch)
{
	ACharacter* DefaultCharacter = CachedPlayerCharacter->GetClass()->GetDefaultObject<ACharacter>();

	if (FromCrouch)
	{
		UCharacterMovementComponent* MovementComponent = DefaultCharacter->GetCharacterMovement();

		CachedPlayerCharacter->GetCapsuleComponent()->SetCapsuleSize(MovementComponent->CrouchedHalfHeight * 0.6f, MovementComponent->CrouchedHalfHeight, true);
	}

	else
	{
		CachedPlayerCharacter->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(),
			DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
	}

	HeightDifference = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() -
		CachedPlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	CachedPlayerCharacter->OnEndProne();

	bIsProning = false;
}

bool UMPBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

void UMPBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	CurrentMantlingParameters = MantlingParameters;

	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UMPBaseCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
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