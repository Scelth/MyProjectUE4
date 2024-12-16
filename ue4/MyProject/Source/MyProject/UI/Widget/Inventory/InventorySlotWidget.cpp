#include "InventorySlotWidget.h"
#include "MyProject/Inventory/Items/InventoryItem.h"
#include "MyProject/Components/CharacterComponents/CharacterInventoryComponent.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"

void UInventorySlotWidget::InitializeItemSlot(FInventorySlot& InventorySlot)
{
	LinkedSlot = &InventorySlot;

	FInventorySlot::FInventorySlotUpdate OnInventorySlotUpdate;
	OnInventorySlotUpdate.BindUObject(this, &UInventorySlotWidget::UpdateView);
	LinkedSlot->BindOnInventorySlotUpdate(OnInventorySlotUpdate);
}

void UInventorySlotWidget::UpdateView()
{
	if (LinkedSlot == nullptr)
	{
		ImageItemIcon->SetBrushFromTexture(nullptr);

		return;
	}

	if (LinkedSlot->Item.IsValid())
	{
		const FInventoryItemDescription& Description = LinkedSlot->Item->GetDescription();
		ImageItemIcon->SetBrushFromTexture(Description.Icon);
	}

	else
	{
		ImageItemIcon->SetBrushFromTexture(nullptr);
	}
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LinkedSlot == nullptr)
	{
		return FReply::Handled();
	}

	if (!LinkedSlot->Item.IsValid())
	{
		return FReply::Handled();
	}

	FKey MouseBtn = InMouseEvent.GetEffectingButton();

	if (MouseBtn == EKeys::RightMouseButton)
	{
		// Some simplification, so as not to complicate the architecture
		// - on instancing item, we use the current pawn as an outer one.
		// In real practice we need use callback for inform item holder what action was do in UI

		TWeakObjectPtr<UInventoryItem> LinkedSlotItem = LinkedSlot->Item;
		AMPBaseCharacter* ItemOwner = Cast<AMPBaseCharacter>(LinkedSlotItem->GetOuter());

		if (LinkedSlotItem->Consume(ItemOwner))
		{
			LinkedSlot->ClearSlot();
		}

		return FReply::Handled();
	}

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

	return Reply.NativeReply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()));

	// Some simplification for not define new widget for drag and drop operation 
	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), GetClass());
	DragWidget->ImageItemIcon->SetBrushFromTexture(LinkedSlot->Item->GetDescription().Icon);

	DragOperation->DefaultDragVisual = DragWidget;
	DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->Payload = LinkedSlot->Item.Get();
	OutOperation = DragOperation;

	LinkedSlot->ClearSlot();
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!LinkedSlot->Item.IsValid())
	{
		LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload));
		LinkedSlot->UpdateSlotState();

		return true;
	}

	return false;
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload));
	LinkedSlot->UpdateSlotState();
}