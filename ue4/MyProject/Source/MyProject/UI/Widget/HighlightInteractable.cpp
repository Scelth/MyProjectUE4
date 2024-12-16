#include "HighlightInteractable.h"
#include "Components/TextBlock.h"

void UHighlightInteractable::SetActionText(FName KeyName)
{
	if (IsValid(ActionText))
	{
		ActionText->SetText(FText::FromName(KeyName));
	}
}
