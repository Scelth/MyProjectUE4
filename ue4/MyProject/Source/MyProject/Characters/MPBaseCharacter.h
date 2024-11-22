#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MPBaseCharacter.generated.h"

#pragma region Mantling Settings
USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float AnimationCorrectionXY = 65.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float AnimationCorrectionZ = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxHeight = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinHeight = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxHeightStartTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinHeightStartTime = 0.5f;
};
#pragma endregion

#pragma region Forward Declaration
class UMPBaseCharacterMovementComponent;
class UCharacterEquipmentComponent;
class UMPCharacterAttributesComponent;
class AInteractiveActor;
#pragma endregion

typedef TArray<AInteractiveActor*, TInlineAllocator<10>> TInteractiveActorsArray;

UCLASS(Abstract, NotBlueprintable)
class MYPROJECT_API AMPBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	TMulticastDelegate<void(bool)> OnAimingStateChangedEvent;

#pragma region Base
	AMPBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
#pragma endregion

#pragma region Axis
	virtual void MoveForward(float Value) {}
	virtual void MoveRight(float Value) {}
	virtual void Turn(float Value) {}
	virtual void LookUp(float Value) {}
	virtual void SwimForward(float Value) {}
	virtual void SwimRight(float Value) {}
	virtual void SwimUp(float Value) {}

	void ClimbLadderUp(float Value);
#pragma endregion

#pragma region Action
	virtual bool CanJumpInternal_Implementation() const override;
	
	virtual void Mantle(bool bForce = false);
	virtual void Jump();
	virtual void ChangeCrouchState();
	virtual void ChangeProneState();
	virtual void StartSprint();
	virtual void StopSprint();

	void StartFire();
	void StopFire();
	void StartAiming();
	void StopAiming();
	void NextItem();
	void PreviousItem();
	void EquipPrimaryItem();
	void ReloadCurrentRangeWeapon();
	void FiringMode();
	void InteractWithLadder();
#pragma endregion

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

#pragma region Interaction
	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);
#pragma endregion

#pragma region Falling
	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;
#pragma endregion

#pragma region Getters
	bool IsAiming() const { return bIsAiming; }
	float GetAimingMovementSpeed() const { return CurrentAimingMovementSpeed; }

#pragma region InverseKinematic
	float GetIKRightFootOffset() const { return IKRightFootOffset; }
	float GetIKLeftFootOffset() const { return IKLeftFootOffset; }
	float GetIKPelvisOffset() const { return IKPelvisOffset; }
#pragma endregion

	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const { return CharacterEquipmentComponent; }
	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const { return CharacterEquipmentComponent; }

	const UMPCharacterAttributesComponent* GetCharacterAttributesComponent() const { return CharacterAttributesComponent; }
	UMPCharacterAttributesComponent* GetCharacterAttributesComponent_Mutable() const { return CharacterAttributesComponent; }

	const UMPBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return MPBaseCharacterMovementComponent; }
	UMPBaseCharacterMovementComponent* GetBaseCharacterMovementComponent_Mutable() const { return MPBaseCharacterMovementComponent; }

	const class ALadder* GetAvailableLadder() const;
#pragma endregion

protected:
#pragma region UPROPERTY
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | IK Setting")
	FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | IK Setting")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKTraceDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKInterpSpeed = 30.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings HighMantleSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings LowMantleSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling", meta = (ClampMin = 0.f, UIMin = 0.f))
	float LowMantleMaxHeight = 125.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	UMPBaseCharacterMovementComponent* MPBaseCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	UMPCharacterAttributesComponent* CharacterAttributesComponent;

	// Damage depending from fall height in meters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	class UCurveFloat* FallDamageCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	UCharacterEquipmentComponent* CharacterEquipmentComponent;
#pragma endregion

#pragma region Sprint
	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnStartSprint();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnStopSprint();

	virtual bool CanSprint();
#pragma endregion

#pragma region Delegate Calling
	virtual void OnDeath();
	virtual void HandleStaminaEvent(bool bIsOutOfStamina);

	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();
#pragma endregion

	bool CanMantle() const;

private:
#pragma region Property
	bool bIsSprintRequested = false;
	bool bIsAiming = false;
	
	float CrouchDifference = 25.f;
	float CurrentAimingMovementSpeed;

	FVector CurrentFallApex;

	TInteractiveActorsArray AvaibleInteractiveActors;
#pragma endregion

#pragma region Inverse Kinematic
	float IKLeftFootOffset = 0.f;
	float IKRightFootOffset = 0.f;
	float IKPelvisOffset = 0.f;

	float CalculateIKParametersForSocketName(const FName& SocketName) const;
	float CalculateIKPelvisOffset();
	void UpdateIKSettings(float DeltaSeconds);
#pragma endregion

	void TryChangeSprintState(float DeltaTime);

	void EnableRagdoll();

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;
};