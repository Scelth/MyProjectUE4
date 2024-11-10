#include "PlayerAttributesWidget.h"
#include <MyProject/Characters/MPBaseCharacter.h>
#include <MyProject/Components/CharacterComponents/MPCharacterAttributesComponent.h>

void UPlayerAttributesWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BindToAttributeChanges();
}

void UPlayerAttributesWidget::BindToAttributeChanges()
{
    AMPBaseCharacter* Character = Cast<AMPBaseCharacter>(GetOwningPlayerPawn());

    if (IsValid(Character))
    {
        UMPCharacterAttributesComponent* Attributes = Character->GetCharacterAttributesComponent();

        if (IsValid(Attributes))
        {
            Attributes->OnHealthChangeEvent.AddUObject(this, &UPlayerAttributesWidget::SetHealthPercent);
            Attributes->OnStaminaChangeEvent.AddUObject(this, &UPlayerAttributesWidget::SetStaminaPercent);
            Attributes->OnOxygenChangeEvent.AddUObject(this, &UPlayerAttributesWidget::SetOxygenPercent);
        }
    }
}

float UPlayerAttributesWidget::GetHealthPercent() const
{
	return HealthProcent;
}

float UPlayerAttributesWidget::GetStaminaPercent() const
{
	return StaminaProcent;
}

float UPlayerAttributesWidget::GetOxygenPercent() const
{
	return OxygenProcent;
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

void UPlayerAttributesWidget::SetHealthPercent(float HealthProcent_In)
{
	HealthProcent = HealthProcent_In;
}

void UPlayerAttributesWidget::SetStaminaPercent(float StaminaProcent_In)
{
	StaminaProcent = StaminaProcent_In;
}

void UPlayerAttributesWidget::SetOxygenPercent(float OxygenProcent_in)
{
	OxygenProcent = OxygenProcent_in;
}