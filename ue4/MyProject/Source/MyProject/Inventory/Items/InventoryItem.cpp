#include "InventoryItem.h"

void UInventoryItem::Initialize(FName DataTableID_In, const FInventoryItemDescription& ItemDescription_In)
{
	DataTableID = DataTableID_In;
	ItemDescription.Icon = ItemDescription_In.Icon;
	ItemDescription.Name = ItemDescription_In.Name;

	bIsInitialized = true;
}
