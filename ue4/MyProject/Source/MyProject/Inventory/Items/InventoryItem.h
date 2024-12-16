#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "InventoryItem.generated.h"

class APickableItem;
class AEquipableItem;
class AMPBaseCharacter;
class UInventoryItem;

USTRUCT(BlueprintType)
struct FInventoryItemDescription : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	UTexture2D* Icon;
};

USTRUCT(BlueprintType)
struct FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<APickableItem> PickableActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<AEquipableItem> EquipableActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	FInventoryItemDescription WeaponItemDescription;
};

USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	TSubclassOf<APickableItem> PickableActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	FInventoryItemDescription InventoryItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	TSubclassOf<UInventoryItem> InventoryItemClass;
};

UCLASS(Blueprintable)
class MYPROJECT_API UInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(FName DataTableID_In, const FInventoryItemDescription& ItemDescription_In);

	virtual bool IsEquipable() const { return bIsEqipable; }
	virtual bool IsConsumable() const { return bIsConsumable; }
	virtual bool Consume(AMPBaseCharacter* ConsumeTarget) PURE_VIRTUAL(UInventoryItem::Consume, return false;)

	FName GetDataTableID() const { return DataTableID; }
	const FInventoryItemDescription& GetDescription() const { return ItemDescription; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	bool bIsEqipable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	bool bIsConsumable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	FName DataTableID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	FInventoryItemDescription ItemDescription;

private:
	bool bIsInitialized = false;
};
