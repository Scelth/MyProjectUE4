#include "PlayerHUDWidget.h"
#include "Blueprint/WidgetTree.h"
#include "PlayerAttributesWidget.h"
#include "ReticleWidget.h"

UPlayerAttributesWidget* UPlayerHUDWidget::GetPlayetAttributesWidget()
{
	return WidgetTree->FindWidget<UPlayerAttributesWidget>(PlayetAttributesWidgetName);
}

UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}