#pragma once

#include "CoreMinimal.h"
#include "MPProjectile.h"
#include "ExplosiveProjectile.generated.h"

class UExplosionComponent;

UCLASS()
class MYPROJECT_API AExplosiveProjectile : public AMPProjectile
{
	GENERATED_BODY()
	
public:
	AExplosiveProjectile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float DetonationTime = 3.8f;

	virtual void OnProjectileLaunched() override;

private:
	FTimerHandle DetonationTimer;

	AController* GetController();

	void OnDetonationTimerElapsed();
};