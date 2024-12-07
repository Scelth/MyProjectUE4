#include "MPAICharacterController.h"
#include "MyProject/AI/Characters/MPAICharacter.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"

void AMPAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (IsValid(InPawn))
	{
		CachedAICharacter = StaticCast<AMPAICharacter*>(InPawn);
	}

	else
	{
		CachedAICharacter = nullptr;
	}
}

void AMPAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);

	if (!CachedAICharacter.IsValid())
	{
		return;
	}

	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	AActor* DamagedActor = GetClosestSensedActor(UAISense_Damage::StaticClass());

	if (IsValid(DamagedActor))
	{
		ClosestActor = DamagedActor;
	}

	if (IsValid(ClosestActor))
	{
		MoveToActor(ClosestActor);
	}

	//CachedAICharacter->SetCurrentTarget(ClosestActor);
}