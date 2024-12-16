#include "PickablePowerups.h"
#include "MyProject/MPTypes.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Utils/MPDataTableUtils.h"
#include "MyProject/Inventory/Items/InventoryItem.h"


APickablePowerups::APickablePowerups()
{
	PowerupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerupMesh"));
	SetRootComponent(PowerupMesh);
}

void APickablePowerups::Interact(AMPBaseCharacter* Character)
{
	FItemTableRow* ItemData = MPDataTableUtils::FindInventoryItemData(GetDataTableID());

	if (ItemData == nullptr)
	{
		return;
	}

	TWeakObjectPtr<UInventoryItem> Item = TWeakObjectPtr<UInventoryItem>(NewObject<UInventoryItem>(Character, ItemData->InventoryItemClass));
	Item->Initialize(DataTableID, ItemData->InventoryItemDescription);

	const bool bPickedUp = Character->PickupItem(Item);

	if (bPickedUp)
	{
		Destroy();
	}
}

FName APickablePowerups::GetActionEventName() const
{
	return ActionInteract;
}
