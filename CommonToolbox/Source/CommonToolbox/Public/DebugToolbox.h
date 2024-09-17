// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "DebugToolbox.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLBOX_API UDebugToolbox : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Draw a debug circle at the hit point on a surface.
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Debug", meta=(AdvancedDisplay=1))
	static void DrawDebugCircleOnHit(const FHitResult MyStructRef, float radius = 40, FLinearColor color = FLinearColor::White, float duration = 0, float thickness = 1,
									 bool showImpactAxis = false);
};
