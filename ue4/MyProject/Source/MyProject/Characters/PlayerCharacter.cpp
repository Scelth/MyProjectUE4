#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyProject/Components/MovementComponents/MPBaseCharacterMovementComponent.h"
#include "MyProject/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include <MyProject/Actors/Equipment/Weapons/RangeWeaponItem.h>

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	InitialSpringArmLength = SpringArmComponent->TargetArmLength;

	Team = ETeams::Player;
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	TurnAtRate(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	LookUpAtRate(Value);
}

void APlayerCharacter::TurnAtRate(float Value)
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	float TurnModifier = IsValid(CurrentRangeWeapon) && IsAiming() ? CurrentRangeWeapon->GetAimTurnModifier() : CameraBaseSpeedMultiplier;
	AddControllerYawInput(Value * TurnModifier);
}

void APlayerCharacter::LookUpAtRate(float Value)
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	float LookUpModifier = IsValid(CurrentRangeWeapon) && IsAiming() ? CurrentRangeWeapon->GetAimLookUpModifier() : CameraBaseSpeedMultiplier;
	AddControllerPitchInput(Value * LookUpModifier);
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

void APlayerCharacter::SwimForward(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpringArm(DeltaTime);
	AimingTimeline.TickTimeline(DeltaTime);
}

void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();

	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (!IsValid(CameraManager))
	{
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();

	if (AimingCurve && CurrentRangeWeapon->GetItemType() != EEquipableItemType::SniperRifle)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateFOV"));

		AimingTimeline.AddInterpFloat(AimingCurve, TimelineCallback);
		AimingTimeline.PlayFromStart();
	}

	else
	{
		CameraManager->SetFOV(CurrentRangeWeapon->GetAimFOV());
	}
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();

	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (!IsValid(CameraManager))
	{
		return;
	}

	if (AimingCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateFOV"));

		AimingTimeline.AddInterpFloat(AimingCurve, TimelineCallback);
		AimingTimeline.ReverseFromEnd();
	}

	else
	{
		CameraManager->SetFOV(DefaultFOV);
	}
}

void APlayerCharacter::UpdateFOV(float Value)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (!IsValid(CameraManager))
	{
		return;
	}

	float NewFOV = FMath::Lerp(DefaultFOV, AimingFOV, Value);
	CameraManager->SetFOV(NewFOV);
}

void APlayerCharacter::UpdateSpringArm(float DeltaTime)
{
	if (IsProningCamera())
	{
		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, ProneSpringArmLength, DeltaTime, CameraInterpSpeed);
	}

	else
	{
		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, InitialSpringArmLength, DeltaTime, CameraInterpSpeed);

		FVector TargetOffset = SpringArmComponent->TargetOffset;
		TargetOffset.Z = FMath::FInterpTo(SpringArmComponent->TargetOffset.Z, 0.f, DeltaTime, CameraInterpSpeed);
		SpringArmComponent->TargetOffset = TargetOffset;
	}
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	if (GetBaseCharacterMovementComponent()->IsMantling())
	{
		return false;
	}

	return bIsCrouched || Super::CanJumpInternal_Implementation();
}

void APlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}

	Super::OnJumped_Implementation();
}
