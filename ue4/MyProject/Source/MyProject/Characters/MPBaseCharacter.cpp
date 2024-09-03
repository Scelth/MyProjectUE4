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

void AMPBaseCharacter::StartSprint()
{
	bIsSprintRequested = true;

	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void AMPBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
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
	if (bIsSprintRequested && !MPBaseCharacterMovementComponent->IsSprinting() && CanSprint())
	{
		MPBaseCharacterMovementComponent->StartSprint();
		OnStartSprint();
	}

	if (!bIsSprintRequested && MPBaseCharacterMovementComponent->IsSprinting())
	{
		MPBaseCharacterMovementComponent->StopSprint();
		OnStopSprint();
	}
}
