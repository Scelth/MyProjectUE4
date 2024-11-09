#include "PlayerAttributesWidget.h"
#include <MyProject/Characters/MPBaseCharacter.h>
#include <MyProject/Components/CharacterComponents/MPCharacterAttributesComponent.h>

float UPlayerAttributesWidget::GetHealthPercent() const
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

float UPlayerAttributesWidget::GetStaminaPercent() const
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

float UPlayerAttributesWidget::GetOxygenPercent() const
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

ESlateVisibility UPlayerAttributesWidget::GetHealthVisibility() const
{
	return GetHealthPercent() < ValueForVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

ESlateVisibility UPlayerAttributesWidget::GetStaminaVisibility() const
{
	return GetStaminaPercent() < ValueForVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

ESlateVisibility UPlayerAttributesWidget::GetOxygenVisibility() const
{
	return GetOxygenPercent() < ValueForVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}