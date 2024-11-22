#include "MPPlayerController.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "Blueprint/UserWidget.h"
#include "MyProject/Components/CharacterComponents/MPCharacterAttributesComponent.h"
#include "MyProject/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "MyProject/UI/Widget/ReticleWidget.h"
#include "MyProject/UI/Widget/PlayerAttributesWidget.h"
#include "MyProject/UI/Widget/PlayerHUDWidget.h"
#include "MyProject/UI/Widget/AmmoWidget.h"

void AMPPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AMPBaseCharacter>(InPawn);

	CreateAndInitializeWidgets();
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
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AMPPlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AMPPlayerController::Jump);
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
	InputComponent->BindAction("FiringMode", EInputEvent::IE_Pressed, this, &AMPPlayerController::FiringMode);
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

void AMPPlayerController::FiringMode()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->FiringMode();
	}
}

void AMPPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}