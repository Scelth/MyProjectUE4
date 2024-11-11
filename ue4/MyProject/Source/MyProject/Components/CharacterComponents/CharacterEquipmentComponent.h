#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyProject/MPTypes.h"
#include "CharacterEquipmentComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentWeaponAmmoChanged, int32)

class ARangeWeaponItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const { return CurrentEquippedWeapon; }

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ARangeWeaponItem> SideArmClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	ARangeWeaponItem* CurrentEquippedWeapon;

private:
	void CreateLoadout();
	
	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	TWeakObjectPtr<class AMPBaseCharacter> CachedBaseCharacter;
};
