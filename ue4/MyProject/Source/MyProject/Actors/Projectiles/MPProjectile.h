#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPProjectile.generated.h"

UCLASS()
class MYPROJECT_API AMPProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AMPProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;
};