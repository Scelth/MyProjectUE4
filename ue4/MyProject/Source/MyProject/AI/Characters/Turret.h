#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyProject/MPTypes.h"
#include "Turret.generated.h"

UENUM(BlueprintType)
enum class ETurretState : uint8
{
	Searching,
	Firing
};

class UWeaponBarellComponent;

UCLASS()
class MYPROJECT_API ATurret : public APawn
{
	GENERATED_BODY()

public:	
	ATurret();

	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	//virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual FVector GetPawnViewLocation() const override;
	virtual FRotator GetViewRotation() const override;

	void SetCurrentTarget(AActor* NewTarget);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* TurretBaseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* TurretBarellComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponBarellComponent* WeaponBarell;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.f, UIMin = 0.f))
	float BaseFiringInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.f, UIMin = 0.f))
	float BaseSearchingRotationRate = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.f, UIMin = 0.f))
	float BarellPitchRotationRate = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxBarellPitchAngle = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinBarellPitchAngle = -30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 0.f, UIMin = 0.f))
	float RateOfFire = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 0.f, UIMin = 0.f))
	float BulletSpreadAngle = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 0.f, UIMin = 0.f))
	float FireDelayTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Team")
	ETeams Team = ETeams::None;

private:
	void SetCurrentTurretState(ETurretState NewState);
	void SearchingMovement(float DeltaTime);
	void FiringMovement(float DeltaTime);
	void MakeShot();

	float GetFireInterval() const { return 60.f / RateOfFire; }

	FTimerHandle ShotTimer;

	ETurretState CurrentTurretState = ETurretState::Searching;

	AActor* CurrentTarget = nullptr;
};
