#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.generated.h"

UCLASS()
class MYPROJECT_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnAimingStateChanged(bool bIsAiming);
};