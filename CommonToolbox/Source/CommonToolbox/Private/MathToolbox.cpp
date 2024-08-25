// Copyright Tyni Boat, Inc. All Rights Reserved.

#include "MathToolbox.h"




bool UMathToolbox::CheckInRange(const FVector2D range, const float value, bool invalidDefault)
{
	if (range.X >= range.Y || !FMath::IsFinite(value))
	{
		return invalidDefault;
	}
	if (range.X <= value && value < range.Y)
		return true;
	return false;
}

double UMathToolbox::TenPowX(const int exponent)
{
	if (exponent <= 0)
		return 1;
	if (exponent == 1)
		return 10;
	double result = 10;
	for (int i = 1; i < exponent; i++)
		result *= 10;
	return result;
}

double UMathToolbox::TwoPowX(const int exponent)
{
	if (exponent <= 0)
		return 1;
	if (exponent == 1)
		return 2;
	double result = 2;
	for (int i = 1; i < exponent; i++)
		result *= 2;
	return result;
}
