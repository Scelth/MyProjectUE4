#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MPAttributeProgressBar.generated.h"

class UProgressBar;

UCLASS()
class MYPROJECT_API UMPAttributeProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProgressPercantage(float Percentage);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
};
