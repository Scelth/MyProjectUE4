// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectBasePawn.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Engine/CollisionProfile.h"
#include "../Components/MovementComponents/MPBasePawnMovementComponent.h"

// Sets default values
AMyProjectBasePawn::AMyProjectBasePawn()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetSphereRadius(50.f);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;

	//MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("Movement component"));
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UMPBasePawnMovementComponent>(TEXT("Movement component"));
	MovementComponent->SetUpdatedComponent(CollisionComponent);
}

// Called to bind functionality to input
void AMyProjectBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyProjectBasePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProjectBasePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AMyProjectBasePawn::Jump);
}

void AMyProjectBasePawn::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyProjectBasePawn::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMyProjectBasePawn::Jump()
{
	checkf(MovementComponent->IsA<UMPBasePawnMovementComponent>(), TEXT("AMyProjectBasePawn::Jump() can work only with UMPBasePawnMovementComponent"));
	UMPBasePawnMovementComponent* BaseMovement = StaticCast<UMPBasePawnMovementComponent*>(MovementComponent);
	BaseMovement->JumpStart();
}