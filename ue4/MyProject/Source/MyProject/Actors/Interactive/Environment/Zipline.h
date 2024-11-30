#pragma once

#include "CoreMinimal.h"
#include "MyProject/Actors/Interactive/InteractiveActor.h"
#include "Zipline.generated.h"

class UStaticMeshComponent;
class UAnimMontage;
class UCapsuleComponent;

UCLASS(Blueprintable)
class MYPROJECT_API AZipline : public AInteractiveActor
{
    GENERATED_BODY()

public:
    AZipline();

    virtual void OnConstruction(const FTransform& Transform) override;

    const UStaticMeshComponent* GetUpperPillarMeshComponent() const { return UpperPillarMeshComponent; }
    const UStaticMeshComponent* GetLowerPillarMeshComponent() const { return LowerPillarMeshComponent; }

protected:
#pragma region UPROPERTY
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parameters")
    float PillarHeight = 100.f;    
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parameters")
    float CableLength = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parameters", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 90.f, UIMax = 90.f))
    float UpperPillarAngleDegrees = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* UpperPillarMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* LowerPillarMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UInstancedStaticMeshComponent* CableMeshComponent;
#pragma endregion

    UCapsuleComponent* GetPillarInteractionCapsuleVolume() const;

private:
    void ScalePillar(UStaticMeshComponent* PillarMeshComponent);
    void ScaleCable();
    void ScaleInteractionCapsule();
};