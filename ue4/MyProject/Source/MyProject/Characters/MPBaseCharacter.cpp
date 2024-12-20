#include "MPBaseCharacter.h"
#include "MyProject/Components/LedgeDetectorComponent.h"
#include "MyProject/Components/MovementComponents/MPBaseCharacterMovementComponent.h"
#include "MyProject/Components/CharacterComponents/MPCharacterAttributesComponent.h"
#include "MyProject/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "MyProject/Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "MyProject/MPTypes.h"
#include "MyProject/Actors/Interactive/Environment/Ladder.h"
#include "MyProject/Actors/Interactive/Environment/Zipline.h"
#include "MyProject/Actors/Interactive/Interface/IInteractable.h"
#include "MyProject/UI/Widget/World/MPAttributeProgressBar.h"
#include "MyProject/Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Curves/CurveVector.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIController.h"
#include "Components/WidgetComponent.h"

#pragma region Base
AMPBaseCharacter::AMPBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMPBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	MPBaseCharacterMovementComponent = Cast<UMPBaseCharacterMovementComponent>(GetCharacterMovement());

	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
	CharacterAttributesComponent = CreateDefaultSubobject<UMPCharacterAttributesComponent>(TEXT("CharacterAttributes"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));
	CharacterInventoryComponent = CreateDefaultSubobject<UCharacterInventoryComponent>(TEXT("CharacterInventory"));
}

void AMPBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterAttributesComponent->OnDeathChangedEvent.AddUObject(this, &AMPBaseCharacter::OnDeath);
	CharacterAttributesComponent->OnOutOfStaminaChangedEvent.AddUObject(this, &AMPBaseCharacter::HandleStaminaEvent);
}

void AMPBaseCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	if (OnInteractableObjectFound.IsBound())
	{
		OnInteractableObjectFound.Unbind();
	}

	Super::EndPlay(Reason);
}

void AMPBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TryChangeSprintState(DeltaTime);

	UpdateIKSettings(DeltaTime);

	TraceLineOfSight();
}

void AMPBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AAIController* AIController = Cast<AAIController>(NewController);

	if (IsValid(AIController))
	{
		FGenericTeamId TeamID((uint8)Team);

		AIController->SetGenericTeamId(TeamID);
	}
}
#pragma endregion

#pragma region Axis
void AMPBaseCharacter::ClimbLadderUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();

		AddMovementInput(LadderUpVector, Value);
	}
}
#pragma endregion

#pragma region Action
#pragma region Mantle
void AMPBaseCharacter::Mantle(bool bForce /*= false*/)
{
	//if (GetBaseCharacterMovementComponent()->IsSliding())
	//{
	//	return;
	//}

	if (!(CanMantle() || bForce))
	{
		return;
	}

	if (GetBaseCharacterMovementComponent()->IsMantling())
	{
		return;
	}

	FLedgeDescription LedgeDescription;

	if (LedgeDetectorComponent->DetectLedge(LedgeDescription))
	{
		FMantlingMovementParameters MantlingParameters;

		MantlingParameters.MantlingCurve = HighMantleSettings.MantlingCurve;
		MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		MantlingParameters.TargetLocation = LedgeDescription.Location;

		float MantlingHeight;

		if (GetCharacterMovement()->IsCrouching())
		{
			ChangeCrouchState();

			float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			FVector LedgeBottom = LedgeDescription.Location - CapsuleHalfHeight * FVector::UpVector;

			MantlingParameters.PlayerBottomLocation = CapsuleHalfHeight;

			MantlingHeight = (LedgeBottom - MantlingParameters.PlayerBottomLocation).Z;
		}

		else
		{
			MantlingHeight = (MantlingParameters.TargetLocation - MantlingParameters.InitialLocation).Z;
		}

		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

		float MinRange;
		float MaxRange;

		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;

		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);

		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		GetBaseCharacterMovementComponent_Mutable()->StartMantle(MantlingParameters);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
	}
}

#pragma region Getter
bool AMPBaseCharacter::CanMantle() const
{
	return !GetBaseCharacterMovementComponent()->IsOnLadder();
}

const FMantlingSettings& AMPBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}
#pragma endregion
#pragma endregion

#pragma region Jump
void AMPBaseCharacter::Jump()
{
	//if (GetBaseCharacterMovementComponent()->IsSliding())
	//{
	//	return;
	//}

	if (GetBaseCharacterMovementComponent()->IsOnWall())
	{
		GetBaseCharacterMovementComponent_Mutable()->DetachFromWall(EDetachFromInteractionMethod::JumpOff);
	}

	if (GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent_Mutable()->StopProne(false);
		ChangeCrouchState();
	}

	else
	{
		if (!GetBaseCharacterMovementComponent()->IsOutOfStamina())
		{
			Super::Jump();
		}
	}
}

bool AMPBaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
}
#pragma endregion

#pragma region Sliding
void AMPBaseCharacter::Sliding()
{
	//if (GetBaseCharacterMovementComponent()->IsSprinting())
	//{
	//	GetBaseCharacterMovementComponent_Mutable()->StartSlide();
	//}
}
#pragma endregion

#pragma region Change Crouch and Prone States
void AMPBaseCharacter::ChangeCrouchState()
{
	if (GetBaseCharacterMovementComponent()->IsProning())
	{
		return;
	}

	//if (GetBaseCharacterMovementComponent()->IsSliding())
	//{
	//	return;
	//}

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
	//if (GetBaseCharacterMovementComponent()->IsSliding())
	//{
	//	return;
	//}

	if (GetCharacterMovement()->IsCrouching() && !GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent_Mutable()->StartProne();
	}

	else if (GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent_Mutable()->StopProne(true);
	}
}

void AMPBaseCharacter::OnStartProne(float HeightAdjust)
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

void AMPBaseCharacter::OnEndProne(float HeightAdjust)
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
#pragma endregion

#pragma region Sprint
void AMPBaseCharacter::StartSprint()
{
	if (GetBaseCharacterMovementComponent()->IsProning())
	{
		return;
	}

	//if (GetBaseCharacterMovementComponent()->IsSliding())
	//{
	//	return;
	//}

	bIsSprintRequested = true;

	if (GetCharacterMovement()->IsCrouching())
	{
		ChangeCrouchState();
	}
}

void AMPBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

bool AMPBaseCharacter::CanSprint()
{
	return true;
}

void AMPBaseCharacter::TryChangeSprintState(float DeltaTime)
{
	//if (GetBaseCharacterMovementComponent()->IsSliding())
	//{
	//	return;
	//}

	if (bIsSprintRequested && !GetBaseCharacterMovementComponent()->IsSprinting() && CanSprint() && !GetBaseCharacterMovementComponent()->IsOutOfStamina())
	{
		GetBaseCharacterMovementComponent_Mutable()->StartSprint();
		OnStartSprint();
	}

	if (!bIsSprintRequested && GetBaseCharacterMovementComponent()->IsSprinting())
	{
		GetBaseCharacterMovementComponent_Mutable()->StopSprint();
		OnStopSprint();
	}
}
#pragma endregion

#pragma region Attack and Aiming
void AMPBaseCharacter::StartFire()
{
	if (GetBaseCharacterMovementComponent()->IsSprinting() && !GetBaseCharacterMovementComponent()->IsSwimming())
	{
		return;
	}

	if (CharacterEquipmentComponent->IsEquipping())
	{
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();

	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StartFire();
	}
}

void AMPBaseCharacter::StopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();

	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
	}
}

void AMPBaseCharacter::StartAiming()
{
	if (!GetBaseCharacterMovementComponent()->IsSprinting() && !GetBaseCharacterMovementComponent()->IsSwimming())
	{
		ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();

		if (!IsValid(CurrentRangeWeapon))
		{
			return;
		}

		bIsAiming = true;

		CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovementMaxSpeed();
		CurrentRangeWeapon->StartAim();

		OnStartAiming();
	}
}

void AMPBaseCharacter::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();

	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAim();
	}

	bIsAiming = false;

	CurrentAimingMovementSpeed = 0.f;

	OnStopAiming();
}

void AMPBaseCharacter::NextItem()
{
	CharacterEquipmentComponent->EquipNextItem();
}

void AMPBaseCharacter::PreviousItem()
{
	CharacterEquipmentComponent->EquipPreviousItem();
}

void AMPBaseCharacter::EquipPrimaryItem()
{
	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
}

void AMPBaseCharacter::ReloadCurrentRangeWeapon()
{
	if (IsValid(CharacterEquipmentComponent->GetCurrentRangeWeapon()))
	{
		CharacterEquipmentComponent->ReloadCurrentWeapon();
	}
}

void AMPBaseCharacter::ChangeFiringMode()
{

}

void AMPBaseCharacter::AddEquipmentItem(const TSubclassOf<AEquipableItem> EquipableItemClass)
{
	CharacterEquipmentComponent->AddEquipmentItem(EquipableItemClass);
}
#pragma endregion

#pragma region Interaction
void AMPBaseCharacter::InteractWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsSliding())
	{
		return;
	}

	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		GetBaseCharacterMovementComponent_Mutable()->DetachFromLadder(EDetachFromInteractionMethod::JumpOff);
	}

	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();

		if (IsValid(AvailableLadder))
		{
			if (AvailableLadder->IsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}

			GetBaseCharacterMovementComponent_Mutable()->AttachToLadder(AvailableLadder);
		}
	}
}

const ALadder* AMPBaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;

	for (const AInteractiveActor* InteractiveActor : AvaibleInteractiveActors)
	{
		if (InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}

	return Result;
}

void AMPBaseCharacter::InteractWithZipline()
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		GetBaseCharacterMovementComponent_Mutable()->DetachFromZipline();
	}

	else
	{
		const AZipline* AvailableZipline = GetAvailableZipline();

		if (IsValid(AvailableZipline))
		{
			GetBaseCharacterMovementComponent_Mutable()->AttachToZipline(AvailableZipline);
		}
	}
}

const AZipline* AMPBaseCharacter::GetAvailableZipline() const
{
	const AZipline* Result = nullptr;

	for (const AInteractiveActor* InteractiveActor : AvaibleInteractiveActors)
	{
		if (InteractiveActor->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(InteractiveActor);
			break;
		}
	}

	return Result;
}

bool AMPBaseCharacter::PickupItem(TWeakObjectPtr<UInventoryItem> ItemToPickup)
{
	bool Result = false;

	if (CharacterInventoryComponent->HasFreeSlot())
	{
		CharacterInventoryComponent->AddItem(ItemToPickup, 1);
		Result = true;
	}

	return Result;
}

void AMPBaseCharacter::UseInventory(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	if (!CharacterInventoryComponent->IsViewVisible())
	{
		CharacterInventoryComponent->OpenViewInventory(PlayerController);
		PlayerController->SetInputMode(FInputModeGameAndUI{});
		PlayerController->bShowMouseCursor = true;
	}

	else
	{
		CharacterInventoryComponent->CloseViewInventory();
		PlayerController->SetInputMode(FInputModeGameAndUI{});
		PlayerController->bShowMouseCursor = false;
	}
}
#pragma endregion

#pragma region Interactable
void AMPBaseCharacter::TraceLineOfSight()
{
	if (!IsPlayerControlled())
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;

	APlayerController* PlayerController = GetController<APlayerController>();
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	FVector ViewDirection = ViewRotation.Vector();
	FVector TraceEnd = ViewLocation + ViewDirection * LineOfSightDistance;

	FHitResult HitResult;

	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility);

	if (LineOfSightObject.GetObject() != HitResult.Actor)
	{
		LineOfSightObject = HitResult.GetActor();

		FName ActionName;

		if (LineOfSightObject.GetInterface())
		{
			ActionName = LineOfSightObject->GetActionEventName();
		}

		else
		{
			ActionName = NAME_None;
		}

		OnInteractableObjectFound.ExecuteIfBound(ActionName);
	}
}

void AMPBaseCharacter::Interact()
{
	if (LineOfSightObject.GetInterface())
	{
		LineOfSightObject->Interact(this);
	}
}
#pragma endregion
#pragma endregion

#pragma region Delegate Calling
void AMPBaseCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetController()->UnPossess();
	DisableInput(nullptr);
	SetActorTickEnabled(false);

	EnableRagdoll();
}

void AMPBaseCharacter::HandleStaminaEvent(bool bIsOutOfStamina)
{
	GetBaseCharacterMovementComponent_Mutable()->SetIsOutOfStamina(bIsOutOfStamina);
}

void AMPBaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void AMPBaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

void AMPBaseCharacter::OnStartAimingInternal()
{
	if (OnAimingStateChangedEvent.IsBound())
	{
		OnAimingStateChangedEvent.Broadcast(true);
	}
}

void AMPBaseCharacter::OnStopAimingInternal()
{
	if (OnAimingStateChangedEvent.IsBound())
	{
		OnAimingStateChangedEvent.Broadcast(false);
	}
}
#pragma endregion

#pragma region Falling
void AMPBaseCharacter::Falling()
{
	GetBaseCharacterMovementComponent_Mutable()->bNotifyApex = true;
}

void AMPBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01f;

	if (IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.GetActor());
	}
}

void AMPBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}
#pragma endregion

#pragma region Interaction
void AMPBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvaibleInteractiveActors.AddUnique(InteractiveActor);
}

void AMPBaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvaibleInteractiveActors.RemoveSingleSwap(InteractiveActor);
}
#pragma endregion

#pragma region IGenericTeamAgentInterface
FGenericTeamId AMPBaseCharacter::GetGenericTeamId() const
{
	return FGenericTeamId((uint8)Team);
}
#pragma endregion

#pragma region Inverse Kinematic
float AMPBaseCharacter::CalculateIKParametersForSocketName(const FName& SocketName) const
{
	float Result = 0.0f;
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	const FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	const FVector TraceEnd = TraceStart - (CapsuleHalfHeight + IKTraceDistance) * FVector::UpVector;
	const FVector FootSizeBox = FVector(1.f, 15.f, 5.f);
	
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	FHitResult HitResult;

	if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), 
		TraceStart, 
		TraceEnd, 
		FootSizeBox, 
		GetMesh()->GetSocketRotation(SocketName), 
		TraceType, 
		true, 
		TArray<AActor*>(), 
		EDrawDebugTrace::None, 
		HitResult, 
		true))
	{
		float CharacterBottom = TraceStart.Z - CapsuleHalfHeight;
		Result = CharacterBottom - HitResult.Location.Z;
	}

	return Result;
}

float AMPBaseCharacter::CalculateIKPelvisOffset()
{
	return  IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset;
}

void AMPBaseCharacter::UpdateIKSettings(float DeltaSeconds)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, CalculateIKParametersForSocketName(RightFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, CalculateIKParametersForSocketName(LeftFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset, CalculateIKPelvisOffset(), DeltaSeconds, IKInterpSpeed);
}
#pragma endregion

void AMPBaseCharacter::EnableRagdoll()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}