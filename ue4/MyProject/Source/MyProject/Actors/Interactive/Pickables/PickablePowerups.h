#pragma once

#include "CoreMinimal.h"
#include "PickableItem.h"
#include "PickablePowerups.generated.h"

class AMPBaseCharacter;

UCLASS(Blueprintable)
class MYPROJECT_API APickablePowerups : public APickableItem
{
	GENERATED_BODY()

public:
	APickablePowerups();

	virtual void Interact(AMPBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PowerupMesh;
};
