#include "AmmoWidget.h"

void UAmmoWidget::UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo)
{
	Ammo = NewAmmo;
	TotalAmmo = NewTotalAmmo;
}

void UAmmoWidget::UpdateThrowablesCount(int32 NewTotalThrowables)
{
	TotalThrowables = NewTotalThrowables;
}