#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerAttributesWidget.generated.h"

UCLASS()
class MYPROJECT_API UPlayerAttributesWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;

	UFUNCTION(BlueprintCallable)
	float GetStaminaPercent() const;

	UFUNCTION(BlueprintCallable)
	float GetOxygenPercent() const;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetHealthVisibility() const;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetStaminaVisibility() const;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetOxygenVisibility() const;

private:
	float ValueForVisibility = 1.f;
};