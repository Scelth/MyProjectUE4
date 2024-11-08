#include "PlayerHUDWidget.h"
#include <MyProject/Characters/MPBaseCharacter.h>
#include <MyProject/Components/CharacterComponents/MPCharacterAttributesComponent.h>

float UPlayerHUDWidget::GetHealthPercent() const
{
	float Result = 1.f;

	APawn* Pawn = GetOwningPlayerPawn();
	AMPBaseCharacter* Character = Cast<AMPBaseCharacter>(Pawn);

	if (IsValid(Character))
	{
		const UMPCharacterAttributesComponent* CharacterAttributes = Character->GetCharacterAttributesComponent();

		Result = CharacterAttributes->GetHealthPercent();
	}

	return Result;
}

float UPlayerHUDWidget::GetStaminaPercent() const
{
	float Result = 1.f;

	APawn* Pawn = GetOwningPlayerPawn();
	AMPBaseCharacter* Character = Cast<AMPBaseCharacter>(Pawn);

	if (IsValid(Character))
	{
		const UMPCharacterAttributesComponent* CharacterAttributes = Character->GetCharacterAttributesComponent();

		Result = CharacterAttributes->GetStaminaPercent();
	}

	return Result;
}

float UPlayerHUDWidget::GetOxygenPercent() const
{
	float Result = 1.f;

	APawn* Pawn = GetOwningPlayerPawn();
	AMPBaseCharacter* Character = Cast<AMPBaseCharacter>(Pawn);

	if (IsValid(Character))
	{
		const UMPCharacterAttributesComponent* CharacterAttributes = Character->GetCharacterAttributesComponent();

		Result = CharacterAttributes->GetOxygenPercent();
	}

	return Result;
}
