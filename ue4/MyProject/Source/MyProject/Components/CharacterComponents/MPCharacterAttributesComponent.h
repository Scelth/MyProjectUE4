#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MPCharacterAttributesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfStaminaEventSignature, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMPCharacterAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMPCharacterAttributesComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent;
	FOutOfStaminaEventSignature OnOutOfStaminaEvent;

	bool IsAlive() { return Health > 0.f; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.f))
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.f))
	float MaxStamina = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.f))
	float MinStaminaToSprint = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.f))
	float StaminaRestoreVelocity = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.f))
	float SprintStaminaConsumptionVelocity = 20.f;

private:
	float Health = 0.f;
	float Stamina = 0.f;
	bool bIsStaminaDepleted = false;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamageActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void UpdateStaminaValue(float DeltaTime);

	TWeakObjectPtr<class AMPBaseCharacter> CachedBaseCharacterOwner;
};
