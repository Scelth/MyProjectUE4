#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProject/MPTypes.h"
#include "EquipableItem.generated.h"

class UAnimMontage;

UCLASS(Abstract, NotBlueprintable)
class MYPROJECT_API AEquipableItem : public AActor
{
	GENERATED_BODY()
	
public:
	EEquipableItemType GetItemType() const { return ItemType; }

	UAnimMontage* GetCharacterEquipAnimMontage() const { return CharacterEquipAnimMontage; }

	FName GetUnequippedSocketName() const { return UnequippedSocketName; }
	FName GetEquippedSocketName() const { return EquippedSocketName; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable Item")
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable Item")
	FName UnequippedSocketName = NAME_None;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable Item")
	FName EquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable Item")
	UAnimMontage* CharacterEquipAnimMontage;
};