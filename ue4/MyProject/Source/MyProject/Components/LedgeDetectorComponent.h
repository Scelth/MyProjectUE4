#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LedgeDetectorComponent.generated.h"

USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Description")
	FVector Location;												
									
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Description")
	FVector LedgeNormal;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Description")
	FRotator Rotation;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API ULedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	TWeakObjectPtr<class ACharacter> CachedCharacterOwner;
	float BaseCapsuleRadius;
	float BaseCapsuleHalfHeight;
	float PlayerBottom;

	bool DetectLedge(OUT FLedgeDescription& LedgeDescription);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection Settings", meta = (UIMin = 0.f, ClampMin = 0.f))
	float MinLedgeHeight = 40.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection Settings", meta = (UIMin = 0.f, ClampMin = 0.f))
	float MaxLedgeHeight = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection Settings", meta = (UIMin = 0.f, ClampMin = 0.f))
	float ForwardCheckDistance = 100.f;
};
