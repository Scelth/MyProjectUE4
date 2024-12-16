#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyProject/MPTypes.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;
typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;

class ARangeWeaponItem;
class AThrowableItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const { return CurrentEquippedWeapon; }
	AThrowableItem* GetCurrentThrowableItem() const { return CurrentThrowableItem; }

	TMulticastDelegate<void(int32, int32)> OnCurrentWeaponAmmoChangedEvent;
	TMulticastDelegate<void(int32, int32)> OnCurrentThrowableCountChangedEvent;
	TMulticastDelegate<void(const AEquipableItem*)> OnCurrentEquippedItemChangedEvent;

	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);
	void EquipItemInSlot(EEquipmentSlots Slot);
	void AttachCurrentItemToEquippedSocket();
	void UnequipCurrentItem();
	void EquipNextItem();
	void EquipPreviousItem();
	void LaunchCurrentThrowableItem();
	void AddEquipmentItem(const TSubclassOf<AEquipableItem> EquipableItemClass);

	bool IsEquipping() const { return bIsEquipping; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ARangeWeaponItem* CurrentEquippedWeapon;

	UPROPERTY()
	AEquipableItem* CurrentEquippedItem;

	UPROPERTY()
	AThrowableItem* CurrentThrowableItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlots> IgnoreSlotsWhileSwitching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	EEquipmentSlots AutoEquipItemInSlot = EEquipmentSlots::None;

private:
	bool bIsEquipping = false;

	TAmunitionArray AmunitionArray;
	TItemsArray ItemsArray;

	TWeakObjectPtr<class AMPBaseCharacter> CachedBaseCharacter;

	EEquipmentSlots CurrentEquippedSlot;
	EEquipmentSlots PreviousEquippedSlot;

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadedHandle;
	FDelegateHandle OnCurrentThrowableCountChangedHandle;

	FTimerHandle EquipTimer;

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);

	void CreateLoadout();
	void EquipAnimationFinished();
	void AutoEquip();
	
	UFUNCTION()
	void OnWeaponReloadComplete();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UFUNCTION()
	void OnCurrentThrowableCountChanged(int32 Throwable);

	int32 GetAvailableAmunitionForCurrentWeapon();
	int32 GetAvailableThrowableCount();
};
