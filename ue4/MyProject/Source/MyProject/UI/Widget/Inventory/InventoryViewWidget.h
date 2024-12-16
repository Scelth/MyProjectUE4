#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryViewWidget.generated.h"

struct FInventorySlot;
class UInventorySlotWidget;
class UGridPanel;

UCLASS()
class MYPROJECT_API UInventoryViewWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeViewWidget(TArray<FInventorySlot>& InventorySlots);

protected:
	void AddItemSlotView(FInventorySlot& SlotToAdd);

	UPROPERTY(meta = (BindWidget))
	UGridPanel* GridPanelItemSlots;

	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Settings")
	TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Settings")
	int32 ColumnCount = 4;
};
