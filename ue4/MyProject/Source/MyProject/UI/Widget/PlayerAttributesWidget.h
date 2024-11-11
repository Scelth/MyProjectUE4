#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerAttributesWidget.generated.h"

UCLASS()
class MYPROJECT_API UPlayerAttributesWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthPercentChanged(float HealthPercent_In);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStaminaPercentChanged(float StaminaPercent_In);

	UFUNCTION(BlueprintImplementableEvent)
	void OnOxygenPercentChanged(float OxygenPercent_In);

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

	UPROPERTY(BlueprintReadWrite)
	float HealthPercent;

	UPROPERTY(BlueprintReadWrite)
	float StaminaPercent;

	UPROPERTY(BlueprintReadWrite)
	float OxygenPercent;

private:
	float ValueForVisibility = 1.f;
};