#include "PlayerHUDWidget.h"
#include "Blueprint/WidgetTree.h"
#include "PlayerAttributesWidget.h"

UPlayerAttributesWidget* UPlayerHUDWidget::GetPlayetAttributesWidget()
{
	return WidgetTree->FindWidget<UPlayerAttributesWidget>(PlayetAttributesWidgetName);
}