// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MPBaseCharacterMovementComponent.generated.h"

UCLASS()
class MYPROJECT_API UMPBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE bool IsSprinting() { return bIsSprinting; }

	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();

	FORCEINLINE bool IsProning() const { return bIsProning; }

	void StartProne();
	void StopProne();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Sprint", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SprintSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Prone")
	float ProneCapsuleRadius = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Prone")
	float ProneCapsuleHalfHeight = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Prone")
	float StandingCapsuleHalfHeight = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ProneSpeed = 150.f;

private:
	bool bIsSprinting = false;
	bool bIsProning = false;
};
