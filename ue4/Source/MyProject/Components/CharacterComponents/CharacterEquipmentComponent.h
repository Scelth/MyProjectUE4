#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyProject/MPTypes.h"
#include "CharacterEquipmentComponent.generated.h"

class ARangeWeaponItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const { return CurrentEquippedWeapon; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ARangeWeaponItem> SideArmClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	ARangeWeaponItem* CurrentEquippedWeapon;

private:
	void CreateLoadout();
	
	TWeakObjectPtr<class AMPBaseCharacter> CachedBaseCharacter;
};
