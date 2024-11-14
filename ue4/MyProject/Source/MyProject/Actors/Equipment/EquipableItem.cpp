#include "EquipableItem.h"

void AEquipableItem::Equip()
{
	if (OnEquipementStateChangedEvent.IsBound())
	{
		OnEquipementStateChangedEvent.Broadcast(true);
	}
}

void AEquipableItem::Unequip()
{
	if (OnEquipementStateChangedEvent.IsBound())
	{
		OnEquipementStateChangedEvent.Broadcast(false);
	}
}