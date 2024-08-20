// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "BonePose.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequenceBase.h"
#include "CollisionShape.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonToolboxBPLibrary.generated.h"


UENUM(BlueprintType)
enum class ESurfaceTraceHitType : uint8
{
	NormalHit,
	InnerHit,
	//The surface impact is offset inward the shape used to trace. 
	OuterHit,
	//The surface impact is offset outward the shape used to trace.
	MAX,
	// Used to filter. to say take all of them.
};


// A hit result hit some augmented properties.
USTRUCT(BlueprintType)
struct COMMONTOOLBOX_API FExpandedHitResult
{
	GENERATED_BODY()

	FORCEINLINE FExpandedHitResult()
	{
	}

	FORCEINLINE FExpandedHitResult(FHitResult Hit, ECollisionResponse Response, ESurfaceTraceHitType Offset = ESurfaceTraceHitType::NormalHit, float depth = 0)
	{
		HitResult = Hit;
		CollisionResponse = Response;
		OffsetType = Offset;
		SurfaceTraceDepth = depth;
	}

	// The actual hit result
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Extend")
	FHitResult HitResult = FHitResult();

	// The collision response from the channel traced from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Extend")
	TEnumAsByte<ECollisionResponse> CollisionResponse = ECR_Ignore;

	// The type of the offset of this trace.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Extend")
	ESurfaceTraceHitType OffsetType = ESurfaceTraceHitType::NormalHit;

	// How far did we "dig" to get this surface
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Extend")
	float SurfaceTraceDepth = 0;
};



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


	/// Check for all collisions at a position and rotation in a direction as overlaps. return true if any collision occurs
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Physic")
	static FORCEINLINE bool ComponentTraceMulti(UPrimitiveComponent* Primitive, ECollisionChannel Channel, TArray<FExpandedHitResult>& outHits, FVector position, FVector direction,
	                                            FQuat rotation,
	                                            bool traceComplex = false, ESurfaceTraceHitType offsetFilter = ESurfaceTraceHitType::MAX, float PenetrationCheck = 0)
	{
		if (Primitive)
		{
			FCollisionQueryParams queryParams = FCollisionQueryParams::DefaultQueryParam;
			queryParams.AddIgnoredActor(Primitive->GetOwner());
			return ComponentTraceMulti_internal(Primitive->GetWorld(), Primitive ? Primitive->GetCollisionShape(0) : FCollisionShape::MakeSphere(1), Channel, outHits, position, direction,
			                                    rotation, traceComplex, queryParams, offsetFilter, PenetrationCheck);
		}
		return false;
	}

	// Convert a hit array from hit results to expanded hit results, calculating offset by the way.
	static void EvaluateOffsetTraces_internal(const TArray<FHitResult> IncomingHits, UPARAM(ref) TArray<FExpandedHitResult>& outgoingHits, ECollisionChannel Channel,
	                                 FCollisionQueryParams& queryParams = FCollisionQueryParams::DefaultQueryParam, ESurfaceTraceHitType offsetFilter = ESurfaceTraceHitType::MAX,
	                                 float PenetrationStep = 0);

	static bool ComponentTraceMulti_internal(UWorld* world, FCollisionShape Shape, ECollisionChannel Channel, TArray<FExpandedHitResult>& outHits, FVector position, FVector direction,
	                                         FQuat rotation,
	                                         bool traceComplex = false, FCollisionQueryParams& queryParams = FCollisionQueryParams::DefaultQueryParam,
	                                         ESurfaceTraceHitType offsetFilter = ESurfaceTraceHitType::MAX, float PenetrationStep = 0);

	static FTraceHandle AsyncComponentTraceMulti_internal(UWorld* world, FCollisionShape Shape, FVector position, FVector direction, FQuat rotation,
	                                                      FTraceDelegate* Result, bool traceComplex = false
	                                                      , FCollisionQueryParams& queryParams = FCollisionQueryParams::DefaultQueryParam);


#pragma endregion

#pragma region Debug

	// Draw a debug circle at the hit point on a surface.
	UFUNCTION(BlueprintCallable, Category = "Common Toolbox | Debug", meta=(AdvancedDisplay=1, NotBlueprintThreadSafe))
	static void DrawDebugCircleOnHit(const FHitResult MyStructRef, float radius = 40, FLinearColor color = FLinearColor::White, float duration = 0, float thickness = 1,
	                                 bool showImpactAxis = false);

#pragma endregion
};
