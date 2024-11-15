#include "CharacterEquipmentComponent.h"
#include "MyProject/Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/MPTypes.h"
#include "MyProject/Actors/Equipment/Throwables/ThrowableItem.h"

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;

	if (IsValid(CurrentEquippedWeapon))
	{
		Result = CurrentEquippedWeapon->GetItemType();
	}

	return Result;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	check(IsValid(CurrentEquippedWeapon));
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();

	if (AvailableAmunition <= 0)
	{
		return;
	}

	CurrentEquippedWeapon->StartReload();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull)
{
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);

	if (NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}

	AmunitionArray[(uint32)CurrentEquippedWeapon->GetAmunitionType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if (bCheckIsFull)
	{
		AvailableAmunition = AmunitionArray[(uint32)CurrentEquippedWeapon->GetAmunitionType()];

		bool bIsFullyReloaded = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();

		if (AvailableAmunition == 0 || bIsFullyReloaded)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if (bIsEquipping)
	{
		return;
	}

	UnequipCurrentItem();

	CurrentEquippedItem = ItemsArray[(uint32)Slot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquippedItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquippedItem);

	if (IsValid(CurrentEquippedItem))
	{
		if (IsValid(CurrentEquippedItem->GetCharacterEquipAnimMontage()))
		{
			bIsEquipping = true;

			float EquipDuration = CachedBaseCharacter->PlayAnimMontage(CurrentEquippedItem->GetCharacterEquipAnimMontage());

			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
		}

		else
		{
			AttachCurrentItemToEquippedSocket();
		}

		CurrentEquippedSlot = Slot;
		CurrentEquippedItem->Equip();
	}

	if (IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponAmmoChangedHandle = CurrentEquippedWeapon->OnAmmoChangedEvent.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadedHandle = CurrentEquippedWeapon->OnReloadCompleteEvent.AddUFunction(this, FName("OnWeaponReloadComplete"));

		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquippedSocketName());
	}
}

void UCharacterEquipmentComponent::UnequipCurrentItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnequippedSocketName());
		CurrentEquippedItem->Unequip();
	}

	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChangedEvent.Remove(OnCurrentWeaponAmmoChangedHandle);
		CurrentEquippedWeapon->OnReloadCompleteEvent.Remove(OnCurrentWeaponReloadedHandle);
	}

	PreviousEquippedSlot = CurrentEquippedSlot;

	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);

	while (CurrentSlotIndex != NextSlotIndex && IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex) && !IsValid(ItemsArray[NextSlotIndex]))
	{
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}

	if (CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 PreviousSlotIndex = PreviousItemsArraySlotIndex(CurrentSlotIndex);

	while (CurrentSlotIndex != PreviousSlotIndex && IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex) && !IsValid(ItemsArray[PreviousSlotIndex]))
	{
		PreviousSlotIndex = PreviousItemsArraySlotIndex(PreviousSlotIndex);
	}

	if (CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	int32 AvailableGrenades = AmunitionArray[(uint32)EAmunitionType::FragGrenades];

	if (AvailableGrenades <= 0)
	{
		return;
	}

	if (IsValid(CurrentThrowableItem))
	{
		CurrentThrowableItem->Throw();

		AmunitionArray[(uint32)EAmunitionType::FragGrenades] -= 1;

		bIsEquipping = false;

		EquipItemInSlot(PreviousEquippedSlot);
	}
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedBaseCharacter = Cast<AMPBaseCharacter>(GetOwner());

	CreateLoadout();
}

uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == ItemsArray.Num() - 1)
	{
		return 0;
	}

	else
	{
		return CurrentSlotIndex + 1;
	}
}

uint32 UCharacterEquipmentComponent::PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == 0)
	{
		return ItemsArray.Num() - 1;
	}

	else
	{
		return CurrentSlotIndex - 1;
	}
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	AmunitionArray.AddZeroed((uint32)EAmunitionType::MAX);

	for (const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount)
	{
		AmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	AmunitionArray[(uint32)EAmunitionType::FragGrenades] = MaxGrenade;

	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);

	for (const TPair<EEquipmentSlots, TSubclassOf<AEquipableItem>>& ItemPair : ItemsLoadout)
	{
		if (!IsValid(ItemPair.Value))
		{
			continue;
		}

		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(ItemPair.Value);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnequippedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		Item->Unequip();

		ItemsArray[(uint32)ItemPair.Key] = Item;
	}
}

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquipping = false;

	AttachCurrentItemToEquippedSocket();
}

int32 UCharacterEquipmentComponent::GetAvailableAmunitionForCurrentWeapon()
{
	check(GetCurrentRangeWeapon());
	return AmunitionArray[(uint32)GetCurrentRangeWeapon()->GetAmunitionType()];
}

int32 UCharacterEquipmentComponent::GetAvailableThrowables()
{
	return AmunitionArray[(uint32)EAmunitionType::FragGrenades];
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon();
}

void UCharacterEquipmentComponent::OnCurrentWeaponAmmoChanged(int32 Ammo)
{
	if (OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(Ammo, GetAvailableAmunitionForCurrentWeapon(), GetAvailableThrowables());
	}
}