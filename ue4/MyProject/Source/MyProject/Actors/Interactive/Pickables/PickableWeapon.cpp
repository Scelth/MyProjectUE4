#include "PickableWeapon.h"
#include "MyProject/MPTypes.h"
#include "MyProject/Utils/MPDataTableUtils.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Inventory/Items/InventoryItem.h"

APickableWeapon::APickableWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

void APickableWeapon::Interact(AMPBaseCharacter* Character)
{
	FWeaponTableRow* WeaponRow = MPDataTableUtils::FindWeaponData(DataTableID);

	if (WeaponRow)
	{
		Character->AddEquipmentItem(WeaponRow->EquipableActorClass);

		Destroy();
	}
}

FName APickableWeapon::GetActionEventName() const
{
	return ActionInteract;
}
