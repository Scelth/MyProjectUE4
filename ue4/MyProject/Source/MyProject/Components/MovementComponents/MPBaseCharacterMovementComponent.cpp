// Fill out your copyright notice in the Description page of Project Settings.


#include "MPBaseCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"

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
    if (!CharacterOwner || !CharacterOwner->GetCapsuleComponent())
    {
        return;
    }

    float CurrentCapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

    bIsProning = true;
    CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(ProneCapsuleRadius, ProneCapsuleHalfHeight);

    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(CharacterOwner))
    {
        PlayerCharacter->OnStartProne(CurrentCapsuleHalfHeight, ProneCapsuleHalfHeight);
    }
}


void UMPBaseCharacterMovementComponent::StopProne()
{
    if (!CharacterOwner || !CharacterOwner->GetCapsuleComponent())
    {
        return;
    }

    float CurrentCapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

    bIsProning = false;
    CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), StandingCapsuleHalfHeight);

    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(CharacterOwner))
    {
        PlayerCharacter->OnEndProne(StandingCapsuleHalfHeight, CurrentCapsuleHalfHeight);
    }
}