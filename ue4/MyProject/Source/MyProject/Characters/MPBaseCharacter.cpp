// Fill out your copyright notice in the Description page of Project Settings.


#include "MPBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/MPBaseCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../Components/LedgeDetectorComponent.h"
#include "Curves/CurveVector.h"

AMPBaseCharacter::AMPBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMPBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	MPBaseCharacterMovementComponent = StaticCast<UMPBaseCharacterMovementComponent*>(GetCharacterMovement());

	CurrentStamina = MaxStamina;

	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
}

void AMPBaseCharacter::ChangeCrouchState()
{
	if (!GetBaseCharacterMovementComponent()->IsProning())
	{
		if (GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
		}

		else
		{
			Crouch();
		}
	}
}

void AMPBaseCharacter::ChangeProneState()
{
	if (GetCharacterMovement()->IsCrouching() && !GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent()->StartProne();
	}

	else if (GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent()->StopProne(true);
	}
}

void AMPBaseCharacter::StartSprint()
{
	if (!GetBaseCharacterMovementComponent()->IsProning())
	{
		bIsSprintRequested = true;

		if (GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
		}
	}
}

void AMPBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void AMPBaseCharacter::Jump()
{
	if (GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent()->StopProne(false);
		UnCrouch();
	}

	else
	{
		if (!GetBaseCharacterMovementComponent()->IsOutOfStamina())
		{
			Super::Jump();
		}
	}
}


bool AMPBaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
}

void AMPBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetBaseCharacterMovementComponent()->IsSprinting())
	{
		RestoreStamina(DeltaTime);
	}

	if (CurrentStamina < MaxStamina)
	{
		FColor StaminaColor = (CurrentStamina == 0) ? FColor::Red : FColor::Yellow;
		GEngine->AddOnScreenDebugMessage(1, 1.0f, StaminaColor, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina));
	}

	TryChangeSprintState(DeltaTime);

	UpdateIKSettings(DeltaTime);
}


void AMPBaseCharacter::Mantle()
{
	FLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->DetectLedge(LedgeDescription))
	{
		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.MantlingCurve = HighMantleSettings.MantlingCurve;
		MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;

		float MantlingHeight = (MantlingParameters.TargetLocation - MantlingParameters.InitialLocation).Z;

		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

		float MinRange;
		float MaxRange;

		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;

		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);

		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		GetBaseCharacterMovementComponent()->StartMantle(MantlingParameters);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
	}
}

bool AMPBaseCharacter::CanSprint()
{
	return true;
}

float AMPBaseCharacter::CalculateIKParametersForSocketName(const FName& SocketName) const
{
	float Result = 0.0f;
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	const FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	const FVector TraceEnd = TraceStart - (CapsuleHalfHeight + IKTraceDistance) * FVector::UpVector;
	const FVector FootSizeBox = FVector(1.f, 15.f, 5.f);
	
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	FHitResult HitResult;

	if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), 
		TraceStart, 
		TraceEnd, 
		FootSizeBox, 
		GetMesh()->GetSocketRotation(SocketName), 
		TraceType, 
		true, 
		TArray<AActor*>(), 
		EDrawDebugTrace::None, 
		HitResult, 
		true))
	{
		float CharacterBottom = TraceStart.Z - CapsuleHalfHeight;
		Result = CharacterBottom - HitResult.Location.Z;
	}

	return Result;
}

float AMPBaseCharacter::CalculateIKPelvisOffset()
{
	return  IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset;
}

void AMPBaseCharacter::UpdateIKSettings(float DeltaSeconds)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, CalculateIKParametersForSocketName(RightFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, CalculateIKParametersForSocketName(LeftFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset, CalculateIKPelvisOffset(), DeltaSeconds, IKInterpSpeed);
}

void AMPBaseCharacter::TryChangeSprintState(float DeltaTime)
{
	if (GetBaseCharacterMovementComponent()->IsSprinting())
	{
		CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaTime;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);

		if (CurrentStamina == 0.f)
		{
			GetBaseCharacterMovementComponent()->SetIsOutOfStamina(true);
		}
	}

	if (bIsSprintRequested && !GetBaseCharacterMovementComponent()->IsSprinting() && CanSprint() && !GetBaseCharacterMovementComponent()->IsOutOfStamina())
	{
		GetBaseCharacterMovementComponent()->StartSprint();
		OnStartSprint();
	}

	if (!bIsSprintRequested && GetBaseCharacterMovementComponent()->IsSprinting())
	{
		GetBaseCharacterMovementComponent()->StopSprint();
		OnStopSprint();
	}
}

void AMPBaseCharacter::RestoreStamina(float DeltaTime)
{
	CurrentStamina += StaminaRestoreVelocity * DeltaTime;
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina);

	if (CurrentStamina > MinStaminaToSprint)
	{
		GetBaseCharacterMovementComponent()->SetIsOutOfStamina(false);
	}
}

const FMantlingSettings& AMPBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}
