// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetMesh()->SetVisibility(false);
	GetMesh()->SetHiddenInGame(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = 1;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

void APlayerCharacter::MoveForward(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::OnStartCrouch(float HalfHeight, float ScaleHalfHeight)
{
	Super::OnStartCrouch(HalfHeight, ScaleHalfHeight);
	SpringArmComponent->TargetOffset += FVector(0.f, 0.f, HalfHeight);
}

void APlayerCharacter::OnEndCrouch(float HalfHeight, float ScaleHalfHeight)
{
	Super::OnEndCrouch(HalfHeight, ScaleHalfHeight);
	SpringArmComponent->TargetOffset -= FVector(0.f, 0.f, HalfHeight);
}

void APlayerCharacter::OnStartProne(float HeightDifference)
{
	SpringArmComponent->AddLocalOffset(FVector(0.f, 0.f, HeightDifference));
}

void APlayerCharacter::OnEndProne(float HeightDifference)
{
	SpringArmComponent->AddLocalOffset(FVector(0.f, 0.f, HeightDifference));
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return bIsCrouched || Super::CanJumpInternal_Implementation();
}

void APlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}
