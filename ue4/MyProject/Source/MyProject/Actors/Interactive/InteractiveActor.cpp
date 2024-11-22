#include "InteractiveActor.h"
#include "MyProject/Characters/MPBaseCharacter.h"
#include "Components/CapsuleComponent.h"

void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractionVolume))
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapBegin);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapEnd);
	}
}

void AInteractiveActor::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}

	AMPBaseCharacter* BaseCharacter = StaticCast<AMPBaseCharacter*>(OtherActor);

	BaseCharacter->RegisterInteractiveActor(this);
}

void AInteractiveActor::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}

	AMPBaseCharacter* BaseCharacter = StaticCast<AMPBaseCharacter*>(OtherActor);

	BaseCharacter->UnregisterInteractiveActor(this);
}

bool AInteractiveActor::IsOverlappingCharacterCapsule(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	AMPBaseCharacter* BaseCharacter = Cast<AMPBaseCharacter>(OtherActor);

	if (!IsValid(BaseCharacter))
	{
		return false;
	}

	if (Cast<UCapsuleComponent>(OtherComp) != BaseCharacter->GetCapsuleComponent())
	{
		return false;
	}

	return true;
}