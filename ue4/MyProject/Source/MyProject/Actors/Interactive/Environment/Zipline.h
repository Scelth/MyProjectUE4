#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/Interactive/InteractiveActor.h"
#include "Zipline.generated.h"

class UStaticMeshComponent;
class UAnimMontage;
class UBoxComponent;

UCLASS(Blueprintable)
class MYPROJECT_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()
	
public:
	AZipline();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	float GetPillarHeight() const { return PillarHeight; }
	UAnimMontage* GetAttachFromTopAnimMontage() const { return AttachFromTopAnimMontage; }

	void ScalePillar(UStaticMeshComponent* PillarMeshComponent, float Height, float Width);

protected:
#pragma region UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parameters")
	float PillarHeight = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parameters")
	float PillarWidth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parameters")
	FVector RightPillarLocation = FVector::ZeroVector;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parameters")
	FVector LeftPillarLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parameters")
	UAnimMontage* AttachFromTopAnimMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* RightPillarMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LeftPillarMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CableMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TopInteractionVolume;
#pragma endregion

	//virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	//virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UBoxComponent* GetPillarInteractionBoxVolume() const;
};
