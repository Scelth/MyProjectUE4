#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MyProject/Actors/Interactive/Interface/IInteractable.h"
#include "Door.generated.h"

UCLASS()
class MYPROJECT_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ADoor();

	virtual void Tick(float DeltaTime) override;
	virtual void Interact(AMPBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;
	virtual bool HasOnInteractionCallback() const;
	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName& FunctionName) override;
	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interactive | Door")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interactive | Door")
	USceneComponent* DoorPivot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interactive | Door")
	UCurveFloat* DoorAnimationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleClosed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleOpened = 90.f;

	//IInteractable::OnInteractionEvent OnInteraction;

private:
	bool bIsOpened = false;

	FTimeline DoorOpenAnimTimeline;

	void InteractWithDoor();

	UFUNCTION()
	void UpdateDoorAnimation(float Delta);

	UFUNCTION()
	void OnDoorAnimationFinished();
};
