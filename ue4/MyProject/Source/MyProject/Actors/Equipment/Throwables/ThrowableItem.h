#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	void Throw();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AMPProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.f, UIMax = 90.f, ClampMin = -90.f, ClampMax = 90.f))
	float ThrowAngleY = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.f, UIMax = 90.f, ClampMin = -90.f, ClampMax = 90.f))
	float ThrowAngleX = 0.f;
};