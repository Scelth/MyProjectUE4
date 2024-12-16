#pragma once

#include "CoreMinimal.h"
#include "MyProject/Inventory/Items/InventoryItem.h"
#include "Medkit.generated.h"

UCLASS()
class MYPROJECT_API UMedkit : public UInventoryItem
{
	GENERATED_BODY()
	
public:
	virtual bool Consume(AMPBaseCharacter* ConsumeTarget) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Medkit")
	float Health = 25.f;
};
