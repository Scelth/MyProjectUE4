#include "MPAICharacter.h"
#include "MyProject/Components/CharacterComponents/AIPatrolingComponent.h"

AMPAICharacter::AMPAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIPatrolingComponent = CreateDefaultSubobject<UAIPatrolingComponent>(TEXT("AIPatroling"));
}