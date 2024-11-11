#include "PlayerAttributesWidget.h"

ESlateVisibility UPlayerAttributesWidget::GetHealthVisibility() const
{
	return HealthPercent < ValueForVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

ESlateVisibility UPlayerAttributesWidget::GetStaminaVisibility() const
{
	return StaminaPercent < ValueForVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

ESlateVisibility UPlayerAttributesWidget::GetOxygenVisibility() const
{
	return OxygenPercent < ValueForVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}