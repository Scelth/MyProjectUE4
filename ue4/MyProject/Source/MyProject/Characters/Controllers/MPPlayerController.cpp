#include "MPPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Components/CharacterComponents/MPCharacterAttributesComponent.h"
#include "MyProject/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "MyProject/UI/Widget/ReticleWidget.h"
#include "MyProject/UI/Widget/PlayerAttributesWidget.h"
#include "MyProject/UI/Widget/PlayerHUDWidget.h"
#include "MyProject/UI/Widget/AmmoWidget.h"
#include "GameFramework/PlayerInput.h"


#pragma region Base
void AMPPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AMPBaseCharacter>(InPawn);

	if (CachedBaseCharacter.IsValid() && IsLocalController())
	{
		CreateAndInitializeWidgets();
		CachedBaseCharacter->OnInteractableObjectFound.BindUObject(this, &AMPPlayerController::OnInteractableObjectFount);
	}
}

void AMPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Axis
	InputComponent->BindAxis("MoveForward", this, &AMPPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMPPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AMPPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AMPPlayerController::LookUp);
	InputComponent->BindAxis("SwimForward", this, &AMPPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AMPPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AMPPlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &AMPPlayerController::ClimbLadderUp);

	// Action
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Pressed, this, &AMPPlayerController::InteractWithLadder);
	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Pressed, this, &AMPPlayerController::InteractWithZipline);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AMPPlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AMPPlayerController::Jump);
	InputComponent->BindAction("Sliding", EInputEvent::IE_Pressed, this, &AMPPlayerController::Sliding);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AMPPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AMPPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AMPPlayerController::StopSprint);
	InputComponent->BindAction("Prone", EInputEvent::IE_Pressed, this, &AMPPlayerController::ChangeProneState);
	InputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &AMPPlayerController::PlayerStartAttack);
	InputComponent->BindAction("Attack", EInputEvent::IE_Released, this, &AMPPlayerController::PlayerStopAttack);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AMPPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AMPPlayerController::StopAiming);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Released, this, &AMPPlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Released, this, &AMPPlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &AMPPlayerController::EquipPrimaryItem);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AMPPlayerController::Reload);
	InputComponent->BindAction("ChangeFiringMode", EInputEvent::IE_Pressed, this, &AMPPlayerController::ChangeFiringMode);
	InputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &AMPPlayerController::Interact);
	InputComponent->BindAction("UseInventory", EInputEvent::IE_Pressed, this, &AMPPlayerController::UseInventory);
}

void AMPPlayerController::CreateAndInitializeWidgets()
{
	if (!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);

		if (IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if (IsValid(PlayerHUDWidget) && CachedBaseCharacter.IsValid())
	{
		UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();
		UPlayerAttributesWidget* PlayerAttributesWidget = PlayerHUDWidget->GetPlayetAttributesWidget();
		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();

		if (IsValid(ReticleWidget))
		{
			CachedBaseCharacter->OnAimingStateChangedEvent.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));

			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentEquippedItemChangedEvent.AddUFunction(ReticleWidget, FName("OnEquippedItemChanged"));
		}

		if (IsValid(PlayerAttributesWidget))
		{
			CachedBaseCharacter->GetCharacterAttributesComponent_Mutable()->OnHealthChangedEvent.AddUFunction(PlayerAttributesWidget, FName("UpdateHealthPercent"));
			CachedBaseCharacter->GetCharacterAttributesComponent_Mutable()->OnStaminaChangedEvent.AddUFunction(PlayerAttributesWidget, FName("UpdateStaminaPercent"));
			CachedBaseCharacter->GetCharacterAttributesComponent_Mutable()->OnOxygenChangedEvent.AddUFunction(PlayerAttributesWidget, FName("UpdateOxygenPercent"));
		}

		if (IsValid(AmmoWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
			CharacterEquipment->OnCurrentThrowableCountChangedEvent.AddUFunction(AmmoWidget, FName("UpdateThrowablesCount"));
		}
	}
}

void AMPPlayerController::OnInteractableObjectFount(FName ActionName)
{
	if (!IsValid(PlayerHUDWidget))
	{
		return;
	}

	TArray<FInputActionKeyMapping> ActionKeys = PlayerInput->GetKeysForAction(ActionName);
	const bool HasAnyKeys = ActionKeys.Num() != 0;

	if (HasAnyKeys)
	{
		FName ActionKey = ActionKeys[0].Key.GetFName();
		PlayerHUDWidget->SetHightlightInteractableActionText(ActionKey);
	}

	PlayerHUDWidget->SetHighlightInteractableVisibility(HasAnyKeys);
}
#pragma endregion

#pragma region Axis
void AMPPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AMPPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AMPPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AMPPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AMPPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AMPPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AMPPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AMPPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}
#pragma endregion 

#pragma region Action
void AMPPlayerController::Sliding()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Sliding();
	}
}

void AMPPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AMPPlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeProneState();
	}
}

void AMPPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void AMPPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AMPPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void AMPPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void AMPPlayerController::PlayerStartAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void AMPPlayerController::PlayerStopAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AMPPlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void AMPPlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void AMPPlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void AMPPlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void AMPPlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void AMPPlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ReloadCurrentRangeWeapon();
	}
}

void AMPPlayerController::ChangeFiringMode()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeFiringMode();
	}
}

void AMPPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void AMPPlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void AMPPlayerController::Interact()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Interact();
	}
}

void AMPPlayerController::UseInventory()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->UseInventory(this);
	}
}
#pragma endregion