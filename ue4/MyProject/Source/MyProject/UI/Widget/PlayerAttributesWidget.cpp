#include "PlayerAttributesWidget.h"

float UPlayerAttributesWidget::GetHealthPercent() const
{
	return HealthPercent;
}

float UPlayerAttributesWidget::GetStaminaPercent() const
{
	return StaminaPercent;
}

float UPlayerAttributesWidget::GetOxygenPercent() const
{
	return OxygenPercent;
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