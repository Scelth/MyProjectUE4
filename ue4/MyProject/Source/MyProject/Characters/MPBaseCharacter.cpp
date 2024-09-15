// Fill out your copyright notice in the Description page of Project Settings.


#include "MPBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/MPBaseCharacterMovementComponent.h"

AMPBaseCharacter::AMPBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMPBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	MPBaseCharacterMovementComponent = StaticCast<UMPBaseCharacterMovementComponent*>(GetCharacterMovement());
	//CurrentStamina = MaxStamina;
}

void AMPBaseCharacter::ChangeCrouchState()
{
	if(!GetBaseCharacterMovementComponent()->IsProning())
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
	if (GetCharacterMovement()->IsCrouching())
	{
		GetBaseCharacterMovementComponent()->StartProne();
		UnCrouch();
	}

	else if (GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent()->StopProne();
		Crouch();
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
		GetBaseCharacterMovementComponent()->StopProne();
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
}

bool AMPBaseCharacter::CanSprint()
{
	return true;
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

	if (bIsSprintRequested && 
		!GetBaseCharacterMovementComponent()->IsSprinting() && 
		CanSprint() && 
		!GetBaseCharacterMovementComponent()->IsOutOfStamina())
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