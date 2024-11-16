#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerAttributesWidget.generated.h"

UCLASS()
class MYPROJECT_API UPlayerAttributesWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	float HealthPercent;

	UPROPERTY(BlueprintReadOnly)
	float StaminaPercent;

	UPROPERTY(BlueprintReadOnly)
	float OxygenPercent;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetHealthVisibility() const;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetStaminaVisibility() const;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetOxygenVisibility() const;

private:
	float ValueForVisibility = 1.f;

	UFUNCTION()
	void UpdateHealthPercent(float NewHealth) { HealthPercent = NewHealth; }

	UFUNCTION()
	void UpdateStaminaPercent(float NewStamina) { StaminaPercent = NewStamina; }

	UFUNCTION()
	void UpdateOxygenPercent(float NewOxygen) { OxygenPercent = NewOxygen; }
};