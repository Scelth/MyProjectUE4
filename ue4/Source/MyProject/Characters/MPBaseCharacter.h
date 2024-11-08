#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MPBaseCharacter.generated.h"

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

class UMPBaseCharacterMovementComponent;
class UCharacterEquipmentComponent;
class UMPCharacterAttributesComponent;

UCLASS(Abstract, NotBlueprintable)
class MYPROJECT_API AMPBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMPBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void MoveForward(float Value) {}
	virtual void MoveRight(float Value) {}
	virtual void Turn(float Value) {}
	virtual void LookUp(float Value) {}

	virtual void ChangeCrouchState();
	virtual void ChangeProneState();

	virtual void StartSprint();
	virtual void StopSprint();

	virtual void Jump();
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void Mantle();

	void StartFire();
	void StopFire();

	void StartAiming();
	void StopAiming();
	bool IsAiming() const { return bIsAiming; }
	float GetAimingMovementSpeed() const { return CurrentAimingMovementSpeed; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

	virtual void SwimForward(float Value) {}
	virtual void SwimRight(float Value) {}
	virtual void SwimUp(float Value) {}

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	float GetIKRightFootOffset() const { return IKRightFootOffset; }
	float GetIKLeftFootOffset() const { return IKLeftFootOffset; }
	float GetIKPelvisOffset() const { return IKPelvisOffset; }

	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;

	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const { return CharacterEquipmentComponent; }
	const UMPCharacterAttributesComponent* GetCharacterAttributesComponent() const { return CharacterAttributesComponent; }

	UMPBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return MPBaseCharacterMovementComponent; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnStartSprint();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnStopSprint();

	virtual bool CanSprint();

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

	virtual void OnDeath();
	virtual void HandleStaminaEvent(bool bIsOutOfStamina);

	// Damage depending from fall height in meters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	class UCurveFloat* FallDamageCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	UCharacterEquipmentComponent* CharacterEquipmentComponent;

	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();

private:
	bool bIsSprintRequested = false;
	bool bIsAiming = false;

	float IKLeftFootOffset = 0.f;
	float IKRightFootOffset = 0.f;
	float IKPelvisOffset = 0.f;
	
	float CrouchDifference = 25.f;
	float CurrentAimingMovementSpeed;

	float CalculateIKParametersForSocketName(const FName& SocketName) const;
	float CalculateIKPelvisOffset();

	void UpdateIKSettings(float DeltaSeconds);
	void TryChangeSprintState(float DeltaTime);

	void EnableRagdoll();

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;

	FVector CurrentFallApex;
};
