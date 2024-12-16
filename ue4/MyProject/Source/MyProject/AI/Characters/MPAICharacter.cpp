#include "MPAICharacter.h"
#include "MyProject/Components/CharacterComponents/AIPatrolingComponent.h"
#include "MyProject/UI/Widget/World/MPAttributeProgressBar.h"
#include "MyProject/Components/CharacterComponents/MPCharacterAttributesComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

AMPAICharacter::AMPAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIPatrolingComponent = CreateDefaultSubobject<UAIPatrolingComponent>(TEXT("AIPatroling"));

	HealthBarProgressComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarProgressComponent"));
	HealthBarProgressComponent->SetupAttachment(GetCapsuleComponent());
}

void AMPAICharacter::BeginPlay()
{
	Super::BeginPlay();
	InitializeHealthProgress();
}

void AMPAICharacter::InitializeHealthProgress()
{
	UMPAttributeProgressBar* Widget = Cast<UMPAttributeProgressBar>(HealthBarProgressComponent->GetUserWidgetObject());

	if (!IsValid(Widget))
	{
		HealthBarProgressComponent->SetVisibility(false);
		return;
	}

	if (IsPlayerControlled() && IsLocallyControlled())
	{
		HealthBarProgressComponent->SetVisibility(false);
	}

	CharacterAttributesComponent->OnHealthChangedEvent.AddUObject(Widget, &UMPAttributeProgressBar::SetProgressPercantage);
	CharacterAttributesComponent->OnDeathChangedEvent.AddLambda([=]() {HealthBarProgressComponent->SetVisibility(false); });
	Widget->SetProgressPercantage(CharacterAttributesComponent->GetHealthPercent());
}