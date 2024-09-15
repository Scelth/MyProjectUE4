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
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(ProneCapsuleRadius, ProneCapsuleHalfHeight);

	float HeightDifference = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ProneCapsuleHalfHeight;

	if (CachedPlayerCharacter = Cast<APlayerCharacter>(CharacterOwner))
	{
		CachedPlayerCharacter->OnStartProne(HeightDifference);
	}

	bIsProning = true;
}

void UMPBaseCharacterMovementComponent::StopProne()
{
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), 
		DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	float HeightDifference = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 
		CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	if (CachedPlayerCharacter = Cast<APlayerCharacter>(CharacterOwner))
	{
		CachedPlayerCharacter->OnEndProne(HeightDifference);
	}

	bIsProning = false;
}

void UMPBaseCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	bIsOutOfStamina = bIsOutOfStamina_In;

	if (bIsOutOfStamina)
	{
		CharacterOwner->GetCharacterMovement()->StopMovementImmediately();
	}
}