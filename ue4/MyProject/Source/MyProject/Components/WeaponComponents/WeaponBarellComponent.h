#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponBarellComponent.generated.h"

USTRUCT(BlueprintType)
struct FDecalInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	FVector DecalSize = FVector(5.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	float DecalLifeTime = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	float DecalFadeOutTime = 5.f;
};

UENUM(BlueprintType)
enum class EHitRegistrationType : uint8
{
	HitScan,
	Projectile
};

class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	void Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes")
	float FiringRange = 5000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes | Damage")
	float DamageAmount = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes", meta = (ClampMin = 1, UIMin = 1))
	int32 BulletsPerShot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes | Hit Registration")
	EHitRegistrationType HitRegistrationType = EHitRegistrationType::HitScan;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes | Hit Registration", meta = (EditCondition = "HitRegistrationType == EHitRegistrationType::Projectile"))
	TSubclassOf<class AMPProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes | VFX")
	UNiagaraSystem* MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes | VFX")
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes | Damage")
	UCurveFloat* FalloffDiagram;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes | Decals")
	FDecalInfo DefaultDecalInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell Attributes | Damage")
	TSubclassOf<class UDamageType> DamageTypeClass;

private:
	float DamageMultiplier = 1.f;
	float DecalFadeOutScreenSize = 0.0001f;

	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& Direction/*, const FVector& ShotStart, const FVector& ShotEnd*/);
	void LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection);

	bool HitScan(const FVector& ShotStart, OUT FVector& ShotEnd, const FVector& ShotDirection);

	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;

	APawn* GetOwningPawn() const;

	AController* GetController() const;
};