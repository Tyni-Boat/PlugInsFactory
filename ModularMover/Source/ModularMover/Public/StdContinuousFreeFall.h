// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TypesLibrary.h"
#include "StdContinuousFreeFall.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover|Movement Modes|Contingents", Abstract)
class MODULARMOVER_API UStdContinuousFreeFall : public UBaseContingentMove
{
	GENERATED_BODY()

public:
	// The Gravity Vector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Free Fall")
	FVector GravityVector = FVector(0, 0, -981);

	// The terminal velocity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Free Fall")
	float TerminalVelocity = 14800;

	// The Planar movement speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Free Fall")
	float PlanarMoveVelocity = 100;

	// The turn speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Free Fall")
	float TurnSpeed = 90;

	
	virtual bool CheckContingentMovement_Implementation(const TArray<FExpandedHitResult>& Surfaces, FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput,
	                                                    const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves, const TArray<FTransientMoveInfos>& TransientMoves,
	                                                    TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const override;

	virtual FMechanicProperties ProcessContingentMovement_Implementation(FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum, const FVector MoveInput,
	                                                                     const FMoverInputPool Inputs, const float DeltaTime) const override;
};
