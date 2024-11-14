#include "AnimNotify_LaunchThrowable.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_LaunchThrowable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AMPBaseCharacter* CharacterOwner = Cast<AMPBaseCharacter>(MeshComp->GetOwner());

	if (!IsValid(CharacterOwner))
	{
		return;
	}

	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->LaunchCurrentThrowableItem();
}