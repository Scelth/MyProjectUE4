#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileHit, const FHitResult&, HitResult, const FVector&, Direction/*, const FVector&, ShotStart, const FVector&, ShotEnd*/);

UCLASS()
class MYPROJECT_API AMPProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(BlueprintAssignable)
	FOnProjectileHit OnProjectileHit;

	AMPProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	virtual void BeginPlay() override;

	virtual void OnProjectileLaunched();

private:
	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};