#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DebugSubsystem.generated.h"

UCLASS()
class MYPROJECT_API UDebugSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	bool IsCategoryEnabled(const FName& CategoryName) const;

private:
	TMap<FName, bool> EnabledDebugCategories;

	UFUNCTION(Exec)
	void EnableDebugCategory(const FName& CategoryName, bool bIsEnabled);
};
