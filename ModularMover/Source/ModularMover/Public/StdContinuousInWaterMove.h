// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TypesLibrary.h"
#include "StdContinuousInWaterMove.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover|Movement Modes|Contingents", Abstract, HideCategories=(Default))
class MODULARMOVER_API UStdContinuousInWaterMove : public UBaseContingentMove
{
	GENERATED_BODY()

public:

	UStdContinuousInWaterMove();
	
	// Water object type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="In Water | Detection")
	TEnumAsByte<ECollisionChannel> WaterChannel = ECC_WorldDynamic;

	// The immersion depth to keep When floating. also serve to trigger in the swimming state.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="In Water | Detection")
	float FloatingHeight = 170;

	// When already in water, if the immersion is lower than this value the mover will no longer will be considered in water.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="In Water | Detection")
	float OutWaterImmersion = 155;

	// When using a buoyancy, input vector input of the water surface location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="In Water | Detection")
	FName WaterSurfaceLocation = NAME_None;


	// The scale of the archimed force.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="In Water | Movement")
	float ArchimedForceScale = 1;

	// The map of Max speed (X), Acceleration (Y), deceleration (Z) and turn speed (W) when moving by value input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="In Water | Movement")
	TMap<FName, FVector4> MoveParams;

	// The default Max speed (X), Acceleration (Y), deceleration (Z) and turn speed (W) when moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="In Water | Movement")
	FVector4 FallBackMoveParams = FVector4(400, 1200, 1200, 400);


	virtual bool CheckContingentMovement_Implementation(UActorComponent* MoverActorComponent, const TArray<FExpandedHitResult>& Surfaces, FContingentMoveInfos& MoveInfos,
	                                                    const FMomentum& CurrentMomentum, const FVector MoveInput,
	                                                    const FMoverInputPool Inputs, const TArray<FContingentMoveInfos>& ContingentMoves, const TArray<FTransientMoveInfos>& TransientMoves,
	                                                    TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const override;

	virtual FMechanicProperties ProcessContingentMovement_Implementation(UActorComponent* MoverActorComponent, FContingentMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                                                                     const FVector MoveInput,
	                                                                     const FMoverInputPool Inputs, const float DeltaTime) const override;

	// Get the actual move params depending on inputs.
	FVector4 GetMoveParams(const FMoverInputPool& Inputs) const;
};
