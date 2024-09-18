// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TypesLibrary.h"
#include "StdContinuousOnGroundMove.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover|Movement Modes|Contingents", Abstract)
class MODULARMOVER_API UStdContinuousOnGroundMove : public UBaseContingentMove
{
	GENERATED_BODY()
public:
	// Maximum distance to detect surface below.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxGroundDistance = 25;

	// Maximum step height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxStepHeight = 35;

	// Maximum slope angle to start sliding (deg)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxWalkableSlopeAngle = 45;

	// The map of Max speed when moving by trigger input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	TMap<FName, float> MoveSpeeds;

	// The default Max speed when moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	float FallBackMoveSpeed = 250;


	virtual bool CheckContingentMovement_Implementation(const TArray<FExpandedHitResult>& Surfaces, FContingentMoveInfos& MoveInfos, const FMomentum CurrentMomentum, const FVector MoveInput,
	                                                    const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves, const TArray<FTransientMoveInfos>& TransientMoves,
	                                                    TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const override;

	virtual FMechanicProperties ProcessContingentMovement_Implementation(FContingentMoveInfos& MoveInfos, const FMomentum CurrentMomentum, const FVector MoveInput,
	                                                                     const FMoverInputPool Inputs, const float DeltaTime) const override;
};
