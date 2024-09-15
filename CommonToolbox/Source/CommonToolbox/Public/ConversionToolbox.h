// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ConversionToolbox.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLBOX_API UConversionToolbox : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Get the number of frame per seconds from a delta time
	UFUNCTION(BlueprintPure, Category = "Toolbox|Conversions", meta=(BlueprintThreadSafe))
	static double DeltaTimeToFPS(double deltaTime);

	// Get the 0-1 value from evaluation of a curve alpha option from a 0-1 input
	UFUNCTION(BlueprintPure, Category = "Toolbox|Conversions", meta=(BlueprintThreadSafe))
	static double EvaluateCurve(FAlphaBlend curve, double inValue);

	// Get the 0-1 value from evaluation of a curve alpha option from a 0-1 input
	UFUNCTION(BlueprintPure, Category = "Toolbox|Conversions", meta=(BlueprintThreadSafe))
	static double EvaluateCurveOption(EAlphaBlendOption curve, double inValue);

	// Convert from a bool array to an integer flag. useful to serialize indexes in an array.
	UFUNCTION(BlueprintPure, Category = "Toolbox|Conversions", meta=(BlueprintThreadSafe))
	static int BoolArrayToFlag(const TArray<bool> array);

	// Convert from an integer flag to an bool array. useful to deserialize indexes in an array.
	UFUNCTION(BlueprintPure, Category = "Toolbox|Conversions", meta=(BlueprintThreadSafe))
	static TArray<bool> FlagToBoolArray(int flag);

	// Convert a bool array to an index array
	UFUNCTION(BlueprintPure, Category = "Toolbox|Conversions", meta=(BlueprintThreadSafe))
	static TArray<int> BoolToIndexesArray(const TArray<bool> array);

	// Convert an int array of indexes to an bool array
	UFUNCTION(BlueprintPure, Category = "Toolbox|Conversions", meta=(BlueprintThreadSafe))
	static TArray<bool> IndexesToBoolArray(const TArray<int> array);

	// Convert an index to integer flag.
	UFUNCTION(BlueprintPure, Category = "Toolbox|Conversions", meta=(BlueprintThreadSafe))
	static int IndexToFlag(const int index);
};
