#include "Medkit.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Components/CharacterComponents/MPCharacterAttributesComponent.h"

bool UMedkit::Consume(AMPBaseCharacter* ConsumeTarget)
{
	ConsumeTarget->GetCharacterAttributesComponent_Mutable()->AddHealth(Health);
	this->ConditionalBeginDestroy();
	return true;
}
