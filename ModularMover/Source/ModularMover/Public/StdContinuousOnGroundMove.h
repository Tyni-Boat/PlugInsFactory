// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TypesLibrary.h"
#include "StdContinuousOnGroundMove.generated.h"


#define FLOATING_HEIGHT 10


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover|Movement Modes|Contingents", Abstract)
class MODULARMOVER_API UStdContinuousOnGroundMove : public UBaseContingentMove
{
	GENERATED_BODY()
public:
	// Ground object type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	TEnumAsByte<ECollisionChannel> GroundChannel = ECC_WorldStatic;
	
	// Maximum distance to detect surface below.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxGroundDistance = 25;

	// Maximum step height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxStepHeight = 35;

	// Maximum surface angle. Pass this value a surface won't even be considered walkable nor slidable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxSurfaceAngle = 70;

	// Maximum slope angle to start sliding (deg)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxSlopeAngle = 45;

	// Minimum slope angle to start sliding (deg)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MinSlopeAngle = 35;

	
	
	// The map of Max speed (X), Acceleration (Y), deceleration (Z) and turn speed (W) when moving by value input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	TMap<FName, FVector4> MoveParams;

	// The default Max speed (X), Acceleration (Y), deceleration (Z) and turn speed (W) when moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	FVector4 FallBackMoveParams = FVector4(500, 2000, 1200, 800);

	// The Sliding Max speed (X), Acceleration (Y), deceleration (Z) and turn speed (W). turn speed will only be effective for above zero values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	FVector4 SlidingMoveParams = FVector4(5500, 1000, 0, 0);

	// The speed to control slide
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	float SlideControlSpeed = 100;

	// Snap speed, the more it is, the snapper will be the movement when climbing a step
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	float SnapSpeed = 25;


	virtual bool CheckContingentMovement_Implementation(const TArray<FExpandedHitResult>& Surfaces, FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput,
	                                                    const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves, const TArray<FTransientMoveInfos>& TransientMoves,
	                                                    TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const override;

	virtual FMechanicProperties ProcessContingentMovement_Implementation(FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput,
	                                                                     const FMoverInputPool Inputs, const float DeltaTime) const override;

	// Get the actual move params depending on inputs.
	FVector4 GetMoveParams(const FMoverInputPool& Inputs) const;
};
