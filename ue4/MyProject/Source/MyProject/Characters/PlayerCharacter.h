#pragma once

#include <Components/TimelineComponent.h>
#include "CoreMinimal.h"
#include "MPBaseCharacter.h"
#include "PlayerCharacter.generated.h"

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

	virtual void OnStartProne(float HeightAdjust);
	virtual void OnEndProne(float HeightAdjust);

	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);

protected:
	bool bIsProningCamera = false;

	float InitialSpringArmLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	float ProneSpringArmLength = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	float CameraInterpSpeed = 2.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Aiming")
	class UCurveFloat* AimingCurve;

	virtual void Tick(float DeltaTime) override;
	virtual void OnStartAimingInternal() override;
	virtual void OnStopAimingInternal() override;

private:
	float CameraBaseSpeedMultiplier = 1.f;
	float DefaultFOV = 90.f; 
	float AimingFOV = 60.f; 

	FTimeline AimingTimeline; 
	
	void UpdateSpringArm(float DeltaTime);
};