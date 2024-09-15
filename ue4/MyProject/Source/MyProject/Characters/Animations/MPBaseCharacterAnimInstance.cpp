// Fill out your copyright notice in the Description page of Project Settings.


#include "MPBaseCharacterAnimInstance.h"
#include "../MPBaseCharacter.h"
#include "../Components/MovementComponents/MPBaseCharacterMovementComponent.h"

void UMPBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AMPBaseCharacter>(), TEXT("UMPBaseCharacterAnimInstance::NativeBeginPlay() can be used only with AMPBaseCharacter"));
	CachedBaseCharacter = StaticCast<AMPBaseCharacter*>(TryGetPawnOwner());
}

void UMPBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}

	UMPBaseCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	Speed = CharacterMovement->Velocity.Size();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsProning = CharacterMovement->IsProning();
	bIsOutOfStamina = CharacterMovement->IsOutOfStamina();
}
