#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/Equipment/EquipableItem.h"
#include "RangeWeaponItem.generated.h"

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	Single,
	FullAuto
};

class UAnimMontage;

UCLASS(Blueprintable)
class MYPROJECT_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	ARangeWeaponItem();

	void StartFire();
	void StopFire();

	void StartAim();
	void StopAim();

	float GetAimFOV() const { return AimFOV; }
	float GetAimMovementMaxSpeed() const { return AimMovementMaxSpeed; }
	float GetAimTurnModifier() const { return AimTurnModifier; }
	float GetAimLookUpModifier() const { return AimLookUpModifier; }

	FTransform GetForeGripTransform() const { return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip); }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWeaponBarellComponent* WeaponBarell;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponFireMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 1.f, UIMin = 1.f))
	float RateOfFire = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters")
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::Single;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float SpreadAngle = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float AimSpreadAngle = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.f, UIMin = 0.f))
	float AimMovementMaxSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 120.f, UIMax = 120.f))
	float AimFOV = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AimTurnModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AimLookUpModifier = 0.5f;

private:
	bool bIsAiming = false;

	float Interval = 60.f;

	float GetShotTimerInterval() const { return Interval / RateOfFire; };
	float GetCurrentBulletSpreadAngle() const;
	float PlayAnimMontage(UAnimMontage* AnimMontage);

	void MakeShot();

	FTimerHandle ShotTimer;

	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;
};