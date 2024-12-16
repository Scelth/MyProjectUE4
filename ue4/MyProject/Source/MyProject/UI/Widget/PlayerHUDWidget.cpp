#include "PlayerHUDWidget.h"
#include "Blueprint/WidgetTree.h"
#include "PlayerAttributesWidget.h"
#include "ReticleWidget.h"
#include "AmmoWidget.h"
#include "HighlightInteractable.h"

UPlayerAttributesWidget* UPlayerHUDWidget::GetPlayetAttributesWidget()
{
	return WidgetTree->FindWidget<UPlayerAttributesWidget>(PlayetAttributesWidgetName);
}

UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

void UPlayerHUDWidget::SetHighlightInteractableVisibility(bool bIsVisible)
{
	if (!IsValid(InteractableKey))
	{
		return;
	}

	if (bIsVisible)
	{
		InteractableKey->SetVisibility(ESlateVisibility::Visible);
	}

	else
	{
		InteractableKey->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerHUDWidget::SetHightlightInteractableActionText(FName KeyName)
{
	if (IsValid(InteractableKey))
	{
		InteractableKey->SetActionText(KeyName);
	}
}