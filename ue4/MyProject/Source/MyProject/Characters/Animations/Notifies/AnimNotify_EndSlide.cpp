#include "AnimNotify_EndSlide.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Components/MovementComponents/MPBaseCharacterMovementComponent.h"

void UAnimNotify_EndSlide::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AMPBaseCharacter* CharacterOwner = Cast<AMPBaseCharacter>(MeshComp->GetOwner());

	if (IsValid(CharacterOwner))
	{
		CharacterOwner->GetBaseCharacterMovementComponent_Mutable()->StopSlide();
	}
}