#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIAttributesComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UAIAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UAIAttributesComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TMulticastDelegate<void()> OnHealthChangedEvent;

	bool IsAlive() { return Health > 0.f; }

	void SetHealth(float NewHealth) { Health = NewHealth; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.f))
	float MaxHealth = 100.f;

private:
	float Health = 0.f;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamageActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	//UFUNCTION()
	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void UpdateHealthValue();

	TWeakObjectPtr<class ATurret> CachedBaseTurretOwner;
};