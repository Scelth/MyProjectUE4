#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterInventoryComponent.generated.h"

class UInventoryItem;
class UInventoryViewWidget;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FInventorySlotUpdate);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UInventoryItem> Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	void BindOnInventorySlotUpdate(const FInventorySlotUpdate& Callback) const;
	void UnbindOnInventorySlotUpdate();
	void UpdateSlotState();
	void ClearSlot();

private:
	mutable FInventorySlotUpdate OnInventorySlotUpdate;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	bool IsViewVisible() const;
	void OpenViewInventory(APlayerController* PlayerController);
	void CloseViewInventory();

	bool HasFreeSlot() const { return ItemsInInventory < Capacity; }
	int32 GetCapacity() const { return Capacity; }

	bool AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count);
	bool RemoveItem(FName ItemID);

	TArray<FInventorySlot> GetAllItemsCopy() const { return InventorySlots; }
	TArray<FText> GetAllItemsNames() const;

protected:
	virtual void BeginPlay() override;

	void CreateViewWidget(APlayerController* PlayerController);

	FInventorySlot* FindItemSlot(FName ItemID);
	FInventorySlot* FindFreeSlot();

	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FInventorySlot> InventorySlots;

	UPROPERTY(EditAnywhere, Category = "View settings")
	TSubclassOf<UInventoryViewWidget> InventoryViewWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory settings", meta = (ClampMin = 1, UIMin = 1))
	int32 Capacity = 16;

private:
	UPROPERTY()
	UInventoryViewWidget* InventoryViewWidget;

	int32 ItemsInInventory;
};
