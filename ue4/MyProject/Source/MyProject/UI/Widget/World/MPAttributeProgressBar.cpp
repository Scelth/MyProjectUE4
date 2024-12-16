#include "MPAttributeProgressBar.h"
#include "Components/ProgressBar.h"

void UMPAttributeProgressBar::SetProgressPercantage(float Percentage)
{
	HealthBar->SetPercent(Percentage);
}