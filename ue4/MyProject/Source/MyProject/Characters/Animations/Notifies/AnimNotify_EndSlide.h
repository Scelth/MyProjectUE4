#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EndSlide.generated.h"

UCLASS()
class MYPROJECT_API UAnimNotify_EndSlide : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};