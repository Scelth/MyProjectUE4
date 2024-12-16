#pragma once

#include "CoreMinimal.h"
#include "MyProject/Inventory/Items/InventoryItem.h"
#include "Adrenaline.generated.h"

UCLASS()
class MYPROJECT_API UAdrenaline : public UInventoryItem
{
	GENERATED_BODY()
	
public:
	virtual bool Consume(AMPBaseCharacter* ConsumeTarget) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Adrenaline")
	float Adrenaline = 75.f;
};