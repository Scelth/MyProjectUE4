#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MPBaseCharacterMovementComponent.generated.h"

#pragma region Mantling Movement Parameters
struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	FVector PlatformRelativeLocation = FVector::ZeroVector;

	float PlayerBottomLocation = 0.f;
	float Duration = 1.f;
	float StartTime = 0.f;

	UCurveVector* MantlingCurve;

	UPrimitiveComponent* Platform = nullptr;
};
#pragma endregion

#pragma region Custom Movement Mode
UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),
	CMOVE_WallRun UMETA(DisplayName = "Wall run"),
	CMOVE_Max UMETA(Hidden)
};
#pragma endregion

#pragma region Detach From Interactives Method
UENUM(BlueprintType)
enum class EDetachFromInteractionMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff
};
#pragma endregion

#pragma region WallRun Side
UENUM(BlueprintType)
enum class EWallRunSide : uint8
{
	None UMETA(DisplayName = "None"),
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left")
};
#pragma endregion

UCLASS()
class MYPROJECT_API UMPBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual float GetMaxSpeed() const override;
	virtual void PhysicsRotation(float DeltaTime) override;

#pragma region Sprint
	bool IsSprinting() const { return bIsSprinting; }
	void StartSprint();
	void StopSprint();

	bool IsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In) { bIsOutOfStamina = bIsOutOfStamina_In; }
#pragma endregion

#pragma region Prone
	bool IsProning() const { return bIsProning; }
	void StartProne();
	void StopProne(bool bIsFromCrouch);
#pragma endregion

#pragma region Mantle
	bool IsMantling() const;
	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
#pragma endregion

#pragma region Ladder
	void AttachToLadder(const class ALadder* Ladder);
	void DetachFromLadder(EDetachFromInteractionMethod DetachFromLadderMethod = EDetachFromInteractionMethod::Fall);
	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	bool IsOnLadder() const;
	float GetLadderSpeedRation() const;
	const class ALadder* GetCurrentLadder() const { return CurrentLadder; }
#pragma endregion

#pragma region WallForRun
	bool IsOnWall() const;
	void AttachToWall(EWallRunSide Side, const FVector& Direction);
	void DetachFromWall(EDetachFromInteractionMethod DetachFromWallMethod = EDetachFromInteractionMethod::Fall);
	void GetWallRunSideAndDirection(const FVector& HitNormal, EWallRunSide& OutSide, FVector& OutDirection) const;
	EWallRunSide GetCurrentWallRunSide() const { return CurrentWallRunSide; }
#pragma endregion

protected:
#pragma region Base
	virtual void BeginPlay() override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	class AMPBaseCharacter* GetBaseCharacterOwner() const;
#pragma endregion

#pragma region Phys
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	void PhysMantling(float DeltaTime, int32 Iterations);
	void PhysLadder(float DeltaTime, int32 Iterations);
	void PhysWallRun(float DeltaTime, int32 iterations);
#pragma endregion

#pragma region UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Sprint", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SprintSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ProneCapsuleRadius = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ProneCapsuleHalfHeight = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ProneSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Swimming", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SwimmingCapsuleRadius = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Swimming", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SwimmingCapsuleHalfHeight = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Ladder", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ClimbingMaxSpeed = 200.f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Ladder", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ClimbingOnLadderBrakingDeceleration = 2048.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Ladder", meta = (ClampMin = 0.f, UIMin = 0.f))
	float LadderToCharacterOffset = 60.f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Ladder", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxLadderTopOffset = 90.f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Ladder", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinLadderBottomOffset = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement | Ladder", meta = (ClampMin = 0.f, UIMin = 0.f))
	float JumpOffFromLadderSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement | WallRun", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxWallRunTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement | WallRun", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxWallRunSpeed = 1000.0f;
#pragma endregion

private:
#pragma region Property
	bool bIsSprinting = false;
	bool bIsProning = false;
	bool bIsOutOfStamina = false;
	bool bIsMantling = false;
	bool bIsForceRotation = false;

	float ScaleCrouchHalfHeight = 0.6f;
	float HeightAdjust = 0.f;
	float InterpSpeed = 10.f;
	float WallRunTimeElapsed = 0.f;
	float ForwardAxis = 0.0f;
	float RightAxis = 0.0f;

	TSoftObjectPtr<class APlayerCharacter> CachedPlayerCharacter;

	FMantlingMovementParameters CurrentMantlingParameters;

	FTimerHandle MantlingTimer;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;

	FVector CurrentWallRunDirection = FVector::ZeroVector;
	EWallRunSide CurrentWallRunSide = EWallRunSide::None;

	const ALadder* CurrentLadder = nullptr;
#pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float ExhaustedSpeed = 200.f;

	UFUNCTION()
	void OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
