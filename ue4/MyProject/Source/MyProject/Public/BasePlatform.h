// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "PlatformInvocator.h"
#include "BasePlatform.generated.h"

UENUM(BlueprintType)
enum class EPlatformBehavior : uint8
{
	OnDemand = 0,
	Loop
};

UCLASS()
class MYPROJECT_API ABasePlatform : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABasePlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	FVector StartLocation;

	FTimeline PlatformTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	UCurveFloat* TimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FORCEINLINE void BeginPlatformTimeline() { PlatformTimeline.Play(); }

	UFUNCTION()
	void PlatformTimelineUpdate(float Value);

	FORCEINLINE void EndPlatformTimeline() { PlatformTimeline.Reverse(); }
};
