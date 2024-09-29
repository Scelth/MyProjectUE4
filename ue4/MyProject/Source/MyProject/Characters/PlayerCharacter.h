// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MPBaseCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MYPROJECT_API APlayerCharacter : public AMPBaseCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;

	virtual void OnStartCrouch(float HalfHeight, float ScaleHalfHeight) override;
	virtual void OnEndCrouch(float HalfHeight, float ScaleHalfHeight) override;

	virtual void OnStartProne();
	virtual void OnEndProne();

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

protected:
	bool bIsProningCamera = false;

	float InitialSpringArmLength;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	float ProneSpringArmLength = 200.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	float CameraInterpSpeed = 2.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class USpringArmComponent* SpringArmComponent;
};