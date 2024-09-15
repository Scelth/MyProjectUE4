// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MPBaseCharacter.generated.h"

class UMPBaseCharacterMovementComponent;

UCLASS(Abstract, NotBlueprintable)
class MYPROJECT_API AMPBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMPBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};

	virtual void ChangeCrouchState();
	virtual void ChangeProneState();

	virtual void StartSprint();
	virtual void StopSprint();

	virtual void Jump();

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UMPBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() { return MPBaseCharacterMovementComponent; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnStartSprint();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnStopSprint();

	virtual bool CanSprint();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinStaminaToSprint = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float StaminaRestoreVelocity = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float SprintStaminaConsumptionVelocity = 20.f;

	UMPBaseCharacterMovementComponent* MPBaseCharacterMovementComponent;

private:
	bool bIsSprintRequested = false;
	float CurrentStamina = MaxStamina;

	void TryChangeSprintState(float DeltaTime);
	void RestoreStamina(float DeltaTime);
};
