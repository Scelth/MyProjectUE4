#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	bool IsThrowing() const { return bIsThrowing; }

	void Throw();
	void SetThrowableCount(int32 NewThrowableCount);

	EAmunitionType GetAmunitionType() const { return AmmunitionType; }

	int32 GetThrowableCount() const { return ThrowableCount; }

	TMulticastDelegate<void(int32)> OnThrowableCountChangedEvent;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AMPProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.f, UIMax = 90.f, ClampMin = -90.f, ClampMax = 90.f))
	float ThrowAngle = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.f, UIMax = 90.f, ClampMin = -90.f, ClampMax = 90.f))
	float ThrowOffsetX = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables | Ammo", meta = (UIMin = 0, ClampMin = 0))
	int32 ThrowableCount = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Throwables", meta = (ClampMin = 0, UIMin = 0))
	EAmunitionType AmmunitionType;

private:
	bool bIsThrowing = false;
};