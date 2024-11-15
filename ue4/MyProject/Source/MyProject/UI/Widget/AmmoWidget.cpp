#include "AmmoWidget.h"

void UAmmoWidget::UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo, int32 NewTotalThrowables)
{
	Ammo = NewAmmo;
	TotalAmmo = NewTotalAmmo;
	TotalThrowables = NewTotalThrowables;
}