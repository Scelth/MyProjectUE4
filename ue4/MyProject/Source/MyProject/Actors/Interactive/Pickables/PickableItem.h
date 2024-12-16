#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickableItem.generated.h"

class IInteractable;

UCLASS(Abstract, NotBlueprintable)
class MYPROJECT_API APickableItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	const FName& GetDataTableID() const { return DataTableID; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName DataTableID = NAME_None;
};