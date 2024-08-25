// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "Components/PrimitiveComponent.h"
#include "CommonToolboxTypeLibrary.generated.h"



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


//---------------------------------------------------------------------------------------------------------------------------------------------------



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


//---------------------------------------------------------------------------------------------------------------------------------------------------

