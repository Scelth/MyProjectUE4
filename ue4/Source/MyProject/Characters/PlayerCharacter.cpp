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

void APlayerCharacter::TurnAtRate(float Value)
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	float TurnModifier = IsValid(CurrentRangeWeapon) && IsAiming() ? CurrentRangeWeapon->GetAimTurnModifier() : 1.f;
	AddControllerYawInput(Value * TurnModifier);
}

void APlayerCharacter::LookUp(float Value)
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	float LookUpModifier = IsValid(CurrentRangeWeapon) && IsAiming() ? CurrentRangeWeapon->GetAimLookUpModifier() : 1.f;
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

void APlayerCharacter::OnStartProne(float HeightAdjust)
{
	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());

	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}

	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightAdjust;
	}

	bIsProningCamera = true;
}

void APlayerCharacter::OnEndProne(float HeightAdjust)
{
	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());

	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}

	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightAdjust;
	}

	bIsProningCamera = false;
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

	if (IsValid(CameraManager))
	{
		ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
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

	if (IsValid(CameraManager))
	{
		ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		CameraManager->UnlockFOV();
	}
}

void APlayerCharacter::UpdateSpringArm(float DeltaTime)
{
	if (bIsProningCamera)
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