// Fill out your copyright notice in the Description page of Project Settings.


#include "MPBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/MPBaseCharacterMovementComponent.h"

AMPBaseCharacter::AMPBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMPBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	MPBaseCharacterMovementComponent = StaticCast<UMPBaseCharacterMovementComponent*>(GetCharacterMovement());
}

void AMPBaseCharacter::ChangeCrouchState()
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

void AMPBaseCharacter::ChangeProneState()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		GetBaseCharacterMovementComponent()->StartProne();
	}

	else if (GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent()->StopProne();
		UnCrouch();
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
	if (!GetBaseCharacterMovementComponent()->IsProning())
	{
		Super::Jump();
	}
}

void AMPBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TryChangeSprintState();
}

bool AMPBaseCharacter::CanSprint()
{
	return true;
}

void AMPBaseCharacter::TryChangeSprintState()
{
	if (bIsSprintRequested && !GetBaseCharacterMovementComponent()->IsSprinting() && CanSprint())
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
