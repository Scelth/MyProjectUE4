#pragma once

#include "CoreMinimal.h"
#include "MyProject/Inventory/Items/InventoryItem.h"

namespace MPDataTableUtils
{
	FWeaponTableRow* FindWeaponData(const FName WeaponID);
	FItemTableRow* FindInventoryItemData(const FName ItemID);
};