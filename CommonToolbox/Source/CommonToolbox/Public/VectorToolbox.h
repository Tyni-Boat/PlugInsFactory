// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "VectorToolbox.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLBOX_API UVectorToolbox : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Restrict a vector inside a cone defined by normal and angle in degrees.
	UFUNCTION(BlueprintPure, Category = "Toolbox|Vectors", meta=(BlueprintThreadSafe))
	static FVector VectorCone(const FVector inVector, const FVector normal, const float alphaAngle = 90);

	// Check if a vector is inside a cone defined by normal and angle in degrees.
	UFUNCTION(BlueprintPure, Category = "Toolbox|Vectors", meta=(BlueprintThreadSafe))
	static bool IsVectorCone(const FVector inVector, const FVector normal, const float alphaAngle = 90);

	// Project a 2D axis into 3D space using a transform. (transform.Fwd * input.Y & transform.Right * input.X)
	UFUNCTION(BlueprintPure, Category = "Toolbox|Vectors", meta=(AdvancedDisplay=2, BlueprintThreadSafe))
	static FVector Project2DInputs(FVector2D input, FTransform transformRelative, FVector planeNormal = FVector(0));

	// Project a 3D Vector onto a plane and conserve it's lenght
	UFUNCTION(BlueprintPure, Category = "Toolbox|Vectors", meta=(BlueprintThreadSafe))
	static FVector Project3DVector(FVector inVector, FVector planeNormal);

	//Get the vector translating point to surface impact point along an axis if provided
	UFUNCTION(BlueprintPure, Category = "Toolbox|Vectors", meta=(BlueprintThreadSafe))
	static FVector GetSnapOnSurfaceVector(const FVector point, const FHitResult surface, const FVector axis);

	//Calculates the intersection point of two lines defined by points ( A1, A2 ) and ( B1, B2 )
	UFUNCTION(BlueprintPure, Category = "Toolbox|Vectors", meta=(BlueprintThreadSafe))
	static bool LineIntersection(const FVector& A1, const FVector& A2, const FVector& B1, const FVector& B2, FVector& IntersectionPoint);
};
