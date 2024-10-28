#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MPBaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.f;
	float StartTime = 0.f;

	UCurveVector* MantlingCurve;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Max UMETA(Hidden)
};

UCLASS()
class MYPROJECT_API UMPBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual float GetMaxSpeed() const override;

	bool IsSprinting() const { return bIsSprinting; }
	void StartSprint();
	void StopSprint();

	bool IsProning() const { return bIsProning; }
	void StartProne();
	void StopProne(bool bIsFromCrouch);

	bool IsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In) { bIsOutOfStamina = bIsOutOfStamina_In; }

	bool IsMantling() const;
	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();

protected:
	virtual void BeginPlay() override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Sprint", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SprintSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ProneCapsuleRadius = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ProneCapsuleHalfHeight = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ProneSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Swimming", meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleRadius = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Swimming", meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleHalfHeight = 60.f;

private:
	bool bIsSprinting = false;
	bool bIsProning = false;
	bool bIsOutOfStamina = false;
	bool bIsMantling = false;

	float ScaleCrouchHalfHeight = 0.6f;
	float HeightAdjust = 0.f;
	float InterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float ExhaustedSpeed = 200.f;

	TSoftObjectPtr<class APlayerCharacter> CachedPlayerCharacter;

	FMantlingMovementParameters CurrentMantlingParameters;

	FTimerHandle MantlingTimer;
};