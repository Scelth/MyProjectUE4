#include "AnimNotify_AttachEquippedItem.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_AttachEquippedItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AMPBaseCharacter* CharacterOwner = Cast<AMPBaseCharacter>(MeshComp->GetOwner());

	if (!IsValid(CharacterOwner))
	{
		return;
	}

	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->AttachCurrentItemToEquippedSocket();
}