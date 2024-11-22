#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/Interactive/InteractiveActor.h"
#include "Ladder.generated.h"

class UStaticMeshComponent;
class UAnimMontage;
class UBoxComponent;

UCLASS(Blueprintable)
class MYPROJECT_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()
	
public:
	ALadder();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	bool IsOnTop() const { return bIsOnTop; }
	float GetLadderHeight() const { return LadderHeight; }
	UAnimMontage* GetAttachFromTopAnimMontage() const { return AttachFromTopAnimMontage; }
	FVector GetAttachFromTopAnimMontageStartingLocation() const;

protected:
#pragma region UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder | Parameters")
	float LadderHeight = 100.f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder | Parameters")
	float LadderWidth = 50.f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder | Parameters")
	float StepsInterval = 25.f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder | Parameters")
	float BottomStepOffset = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder | Parameters")
	UAnimMontage* AttachFromTopAnimMontage;

	// Offset from ladder's top for starting anim montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder | Parameters")
	FVector AttachFromTopAnimMontageInitialOffset = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* RightRailMeshComponent;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LeftRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInstancedStaticMeshComponent* StepsMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TopInteractionVolume;
#pragma endregion

	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UBoxComponent* GetLadderInteractionBoxVolume() const;

private:
	bool bIsOnTop = false;
};