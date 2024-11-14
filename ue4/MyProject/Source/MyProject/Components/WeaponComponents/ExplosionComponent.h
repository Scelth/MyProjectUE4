#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ExplosionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplosion);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UExplosionComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnExplosion OnExplosion;

	UFUNCTION(BlueprintCallable)
	void Explode(AController* Controller);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxDamage = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinDamage = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage", meta = (ClampMin = 0.f, UIMin = 0.f))
	float DamageFalloff = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Radius", meta = (ClampMin = 0.f, UIMin = 0.f))
	float InnerRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Radius", meta = (ClampMin = 0.f, UIMin = 0.f))
	float OuterRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	TSubclassOf<class UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | VFX")
	UParticleSystem* ExplosionVFX;
};
