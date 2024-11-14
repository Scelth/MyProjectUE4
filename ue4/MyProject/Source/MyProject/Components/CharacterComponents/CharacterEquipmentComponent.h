#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyProject/MPTypes.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;
typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;

class ARangeWeaponItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const { return CurrentEquippedWeapon; }

	TMulticastDelegate<void(int32, int32)> OnCurrentWeaponAmmoChangedEvent;

	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	void EquipItemInSlot(EEquipmentSlots Slot);
	void AttachCurrentItemToEquippedSocket();
	void UnequipCurrentItem();
	void EquipNextItem();
	void EquipPreviousItem();

	bool IsEquipping() const { return bIsEquipping; }

protected:
	UPROPERTY()
	ARangeWeaponItem* CurrentEquippedWeapon;

	UPROPERTY()
	AEquipableItem* CurrentEquippedItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	virtual void BeginPlay() override;

private:
	TAmunitionArray AmunitionArray;
	TItemsArray ItemsArray;

	TWeakObjectPtr<class AMPBaseCharacter> CachedBaseCharacter;

	EEquipmentSlots CurrentEquippedSlot;

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadedHandle;

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);

	FTimerHandle EquipTimer;

	bool bIsEquipping = false;

	void CreateLoadout();
	void EquipAnimationFinished();
	
	UFUNCTION()
	void OnWeaponReloadComplete();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	int32 GetAvailableAmunitionForCurrentWeapon();
};
