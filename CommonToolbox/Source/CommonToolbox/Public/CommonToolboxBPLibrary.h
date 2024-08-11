// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "BonePose.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequenceBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonToolboxBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class COMMONTOOLBOX_API UCommonToolboxBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

#pragma region Maths


	// Check if value is in range (rangeX <= Value < rangeY). if range is invalid (rangeX >= rangeY) return invalidDefault
	UFUNCTION(BlueprintPure, Category = "Common Toolbox | Math", meta=(BlueprintThreadSafe))
	static bool CheckInRange(const FVector2D range, const float value, bool invalidDefault = false);

	// Returns a power of ten for positive values only.
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Math", meta = (CompactNodeTitle = "10powX", BlueprintThreadSafe))
	static double TenPowX(const int exponent);

	// Returns a power of two for positive values only.
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Math", meta = (CompactNodeTitle = "2powX", BlueprintThreadSafe))
	static double TwoPowX(const int exponent);

#pragma endregion

#pragma region Conversions

	// Get the number of frame per seconds from a delta time
	UFUNCTION(BlueprintCallable, Category = "Function Library | Conversions", meta=(BlueprintThreadSafe))
	static double GetFPS(double deltaTime);

	// Convert from a bool array to an integer flag. useful to serialize indexes in an array.
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Conversions", meta=(BlueprintThreadSafe))
	static int BoolArrayToFlag(const TArray<bool> array);

	// Convert from an integer flag to an bool array. useful to deserialize indexes in an array.
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Conversions", meta=(BlueprintThreadSafe))
	static TArray<bool> FlagToBoolArray(int flag);

	// Convert a bool array to an index array
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Conversions", meta=(BlueprintThreadSafe))
	static TArray<int> BoolToIndexesArray(const TArray<bool> array);

	// Convert an int array of indexes to an bool array
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Conversions", meta=(BlueprintThreadSafe))
	static TArray<bool> IndexesToBoolArray(const TArray<int> array);

	// Convert an index to integer flag.
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Conversions", meta=(BlueprintThreadSafe))
	static int IndexToFlag(const int index);

#pragma endregion

#pragma region Objects
#pragma endregion

#pragma region Vectors

	// Restrict a vector inside a cone defined by normal and angle in degrees.
	UFUNCTION(BlueprintPure, Category = "Common Toolbox | Vectors", meta=(BlueprintThreadSafe))
	static FVector VectorCone(const FVector inVector, const FVector normal, const float alphaAngle = 90);

	// Check if a vector is inside a cone defined by normal and angle in degrees.
	UFUNCTION(BlueprintPure, Category = "Common Toolbox | Vectors", meta=(BlueprintThreadSafe))
	static bool IsVectorCone(const FVector inVector, const FVector normal, const float alphaAngle = 90);

	// Project a 2D axis into 3D space using a transform. (transform.Fwd * input.Y & transform.Right * input.X)
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Vectors", meta=(AdvancedDisplay=2, BlueprintThreadSafe))
	static FVector Project2DInputs(FVector2D input, FTransform transformRelative, FVector planeNormal = FVector(0));

	//Get the vector translating point to surface impact point along an axis if provided
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Vectors", meta=(BlueprintThreadSafe))
	static FVector GetSnapOnSurfaceVector(const FVector point, const FHitResult surface, const FVector axis);

#pragma endregion

#pragma region Arrays
#pragma endregion

#pragma region Animation

	static void ExtractLocalSpacePose(const UAnimSequenceBase* Animation, const FBoneContainer& BoneContainer, float Time, bool bExtractRootMotion, FCompactPose& OutPose);

	static void ExtractComponentSpacePose(const UAnimSequenceBase* Animation, const FBoneContainer& BoneContainer, float Time, bool bExtractRootMotion, FCSPose<FCompactPose>& OutPose);

	static FTransform ExtractRootMotionFromAnimation(const UAnimSequenceBase* Animation, float StartTime, float EndTime, TArray<FTransform>* stepArray = nullptr);

	static FTransform ExtractRootTransformFromAnimation(const UAnimSequenceBase* Animation, float Time);

	// Get the weight of the montage specified or of the active montage if NULL Montage.
	UFUNCTION(BlueprintPure, Category = "Common Toolbox | Animation")
	static double GetMontageCurrentWeight(const UAnimInstance* AnimInstance, const UAnimMontage* Montage);

#pragma endregion

#pragma region Physic

	// Compare 2 collision shapes and return true if they are of same type and size.
	static bool CollisionShapeEquals(const FCollisionShape shapeA, const FCollisionShape shapeB);


	// Get the Kinetic Energy of a body of mass (Kg), traveling at velocity. Set distance travelled to get the force.
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Physic")
	static FVector GetKineticEnergy(const FVector velocity, const float mass, const double distanceTraveled = 1);

#pragma endregion

#pragma region Debug

	// Draw a debug circle at the hit point on a surface.
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Debug", meta=(AdvancedDisplay=1, NotBlueprintThreadSafe))
	static void DrawDebugCircleOnHit(const FHitResult MyStructRef, float radius = 40, FLinearColor color = FLinearColor::White, float duration = 0, float thickness = 1,
	                                 bool showImpactAxis = false);

#pragma endregion
};
