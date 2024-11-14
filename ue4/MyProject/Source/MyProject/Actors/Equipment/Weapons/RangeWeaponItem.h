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

UENUM(BlueprintType)
enum class EReloadType : uint8
{
	FullClip,
	ByBullet
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

	int32 GetAmmo() const { return Ammo; }
	int32 GetMaxAmmo() const { return MaxAmmo; }

	void StartReload();
	void EndReload(bool bIsSuccess);

	void SetAmmo(int32 NewAmmo);

	bool CanShoot() const { return Ammo > 0; }

	FTransform GetForeGripTransform() const { return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip); }
	EAmunitionType GetAmunitionType() const { return AmmoType; }

	TMulticastDelegate<void(int32)> OnAmmoChangedEvent;
	TMulticastDelegate<void()> OnReloadCompleteEvent;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWeaponBarellComponent* WeaponBarell;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponFireMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponReloadMontage;

	// FullClip reload type adds ammo only whe the whole reload animation is successfully played
	// ByBullet reload type requires section "EndReload" in character reload animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	EReloadType ReloadType = EReloadType::FullClip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters")
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::Single;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo", meta = (ClampMin = 1, UIMin = 1))
	EAmunitionType AmmoType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 1.f, UIMin = 1.f))
	float RateOfFire = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float SpreadAngle = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float AimSpreadAngle = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.f, UIMin = 0.f))
	float AimMovementMaxSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 120.f, UIMax = 120.f))
	float AimFOV = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float AimTurnModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float AimLookUpModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo", meta = (ClampMin = 1, UIMin = 1))
	int32 MaxAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
	bool bIsAutoReload = true;

private:
	int32 Ammo = 0;

	bool bIsAiming = false;
	bool bIsReloading = false;
	bool bIsFiring = false;

	float Interval = 60.f;

	float GetShotTimerInterval() const { return Interval / RateOfFire; };
	float GetCurrentBulletSpreadAngle() const;

	float PlayAnimMontage(UAnimMontage* AnimMontage);
	void StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime);

	void MakeShot();
	void OnShotTimerElapsed();

	FTimerHandle ShotTimer;
	FTimerHandle ReloadTimer;
};