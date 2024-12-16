#include "Adrenaline.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "MyProject/Components/CharacterComponents/MPCharacterAttributesComponent.h"

bool UAdrenaline::Consume(AMPBaseCharacter* ConsumeTarget)
{
	ConsumeTarget->GetCharacterAttributesComponent_Mutable()->AddStamina(Adrenaline);
	this->ConditionalBeginDestroy();
	return true;
}
