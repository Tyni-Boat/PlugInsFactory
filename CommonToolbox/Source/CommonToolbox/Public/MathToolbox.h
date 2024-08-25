// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MathToolbox.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLBOX_API UMathToolbox : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Check if value is in range (rangeX <= Value < rangeY). if range is invalid (rangeX >= rangeY) return invalidDefault
	UFUNCTION(BlueprintPure, Category = "Toolbox|Math", meta=(BlueprintThreadSafe))
	static bool CheckInRange(const FVector2D range, const float value, bool invalidDefault = false);

	// Returns a power of ten for positive values only.
	UFUNCTION(BlueprintCallable, Category = "Toolbox|Math", meta = (CompactNodeTitle = "10powX", BlueprintThreadSafe))
	static double TenPowX(const int exponent);

	// Returns a power of two for positive values only.
	UFUNCTION(BlueprintCallable, Category = "Toolbox|Math", meta = (CompactNodeTitle = "2powX", BlueprintThreadSafe))
	static double TwoPowX(const int exponent);
};
