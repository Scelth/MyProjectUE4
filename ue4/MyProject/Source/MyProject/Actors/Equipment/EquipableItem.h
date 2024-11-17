#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProject/MPTypes.h"
#include "EquipableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

class UAnimMontage;

UCLASS(Abstract, NotBlueprintable)
class MYPROJECT_API AEquipableItem : public AActor
{
	GENERATED_BODY()
	
public:
	virtual void Equip();
	virtual void Unequip();

	EEquipableItemType GetItemType() const { return ItemType; }

	UAnimMontage* GetCharacterEquipAnimMontage() const { return CharacterEquipAnimMontage; }

	FName GetUnequippedSocketName() const { return UnequippedSocketName; }
	FName GetEquippedSocketName() const { return EquippedSocketName; }

	virtual EReticleType GetReticleType() const { return ReticleType; }
	
protected:
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentStateChanged OnEquipementStateChangedEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable Item")
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable Item")
	FName UnequippedSocketName = NAME_None;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable Item")
	FName EquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable Item")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable Item | Reticle")
	EReticleType ReticleType = EReticleType::None;
};