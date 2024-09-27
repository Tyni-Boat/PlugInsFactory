// Copyright Tyni Boat, Inc. All Rights Reserved.

#include "ConversionToolbox.h"
#include "MathToolbox.h"


double UConversionToolbox::DeltaTimeToFPS(double deltaTime)
{
	if (FMath::IsNearlyZero(deltaTime))
		return 0;
	return 1 / deltaTime;
}

double UConversionToolbox::EvaluateCurve(FAlphaBlend curve, double inValue)
{
	return FAlphaBlend::AlphaToBlendOption(inValue, curve.GetBlendOption(), curve.GetCustomCurve());
}

double UConversionToolbox::EvaluateCurveOption(EAlphaBlendOption curve, double inValue)
{
	return FAlphaBlend::AlphaToBlendOption(inValue, curve);
}


int UConversionToolbox::BoolArrayToFlag(const TArray<bool> array)
{
	//Translate to int
	TArray<int> arr;
	for (int i = 0; i < array.Num(); i++)
		arr.Add(array[i] ? 1 : 0);

	int result = 0;
	for (int i = 0; i < arr.Num(); i++)
	{
		result += arr[i] * UMathToolbox::TwoPowX(i);
	}
	return result;
}


TArray<bool> UConversionToolbox::FlagToBoolArray(int flag)
{
	TArray<int> binary_array;
	TArray<bool> bool_array;
	int n = flag;

	while (n > 0)
	{
		binary_array.Add(n % 2);
		n /= 2;
	}
	for (int i = binary_array.Num() - 1; i >= 0; i--)
		bool_array.Add(binary_array[binary_array.Num() - (i + 1)] >= 1);

	return bool_array;
}


TArray<int> UConversionToolbox::BoolToIndexesArray(const TArray<bool> array)
{
	TArray<int> indexArray;
	//loop the array and add to the int[] i if array[i] == true
	for (int i = 0; i < array.Num(); i++)
		if (array[i])
			indexArray.Add(i);

	return indexArray;
}


TArray<bool> UConversionToolbox::IndexesToBoolArray(const TArray<int> array)
{
	//Get the highest index
	int length = -1;
	for (int i = 0; i < array.Num(); i++)
		if (array[i] > length)
			length = array[i];

	//set the bool[] size
	TArray<bool> bools = TArray<bool>();
	for (int i = 0; i < (length + 1); i++)
		bools.Add(false);
	//loop the array and true at bool[x], where x = arr[i]
	for (int i = 0; i < array.Num(); i++)
	{
		if (!bools.IsValidIndex(array[i]))
			continue;
		bools[array[i]] = true;
	}

	return bools;
}


int UConversionToolbox::IndexToFlag(const int index)
{
	if (index < 0)
		return 0;
	return UMathToolbox::TwoPowX(index);
}

AActor* UConversionToolbox::GetComponentActor(const UActorComponent* Component)
{
	return Component ? Component->GetOwner() : nullptr;
}
