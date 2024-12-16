#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/Interactive/Pickables/PickableItem.h"
#include "PickableWeapon.generated.h"

class AMPBaseCharacter;

UCLASS(Blueprintable)
class MYPROJECT_API APickableWeapon : public APickableItem
{
	GENERATED_BODY()
	
public:
	APickableWeapon();

	virtual void Interact(AMPBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponMesh;
};
