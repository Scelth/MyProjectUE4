#include "MPDataTableUtils.h"
#include "Engine/DataTable.h"
#include "MyProject/MPTypes.h"
#include "MyProject/Inventory/Items/InventoryItem.h"

FWeaponTableRow* MPDataTableUtils::FindWeaponData(const FName WeaponID)
{
	static const FString ContextString(TEXT("Find Weapon Data"));

	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/MyProject/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));

	if (WeaponDataTable == nullptr)
	{
		return nullptr;
	}

	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, ContextString);
}

FItemTableRow* MPDataTableUtils::FindInventoryItemData(const FName ItemID)
{
	static const FString ContextString(TEXT("Find Item Data"));

	UDataTable* InventoryItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/MyProject/Core/Data/DataTables/DT_InventoryItemList.DT_InventoryItemList"));

	if (InventoryItemDataTable == nullptr)
	{
		return nullptr;
	}

	return InventoryItemDataTable->FindRow<FItemTableRow>(ItemID, ContextString);
}
