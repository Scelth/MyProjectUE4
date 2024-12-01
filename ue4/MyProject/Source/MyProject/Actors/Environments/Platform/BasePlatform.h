#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "BasePlatform.generated.h"

UENUM(BlueprintType)
enum class EPlatformBehavior : uint8
{
	OnDemand = 0,
	Loop = 1
};

UCLASS()
class MYPROJECT_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
    ABasePlatform();

protected:
	bool bIsReversing = false;

	FTimerHandle LoopDelayTimerHandle;

	FTimeline PlatformTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LoopDelay = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* TimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void PlatformTimelineUpdate(float Alpha);

	void HandleLoopDelay();
	void StartMovement();
};
