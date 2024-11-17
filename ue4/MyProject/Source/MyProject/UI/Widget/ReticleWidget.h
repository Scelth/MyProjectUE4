#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyProject/MPTypes.h"
#include "ReticleWidget.generated.h"

class AEquipableItem;

UCLASS()
class MYPROJECT_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reticle")
	EReticleType CurrentReticle = EReticleType::None;

	UFUNCTION(BlueprintNativeEvent)
	void OnAimingStateChanged(bool bIsAiming);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquippedItemChanged(const AEquipableItem* EquipableItem);

private:
	TWeakObjectPtr<const AEquipableItem> CurrentEquippedItem;

	void SetupCurrentReticle();
};