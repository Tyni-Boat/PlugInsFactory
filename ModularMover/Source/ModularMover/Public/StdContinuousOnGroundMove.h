// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TypesLibrary.h"
#include "StdContinuousOnGroundMove.generated.h"


#define FLOATING_HEIGHT 10


/**
 *  Priority 6 Standard On ground movement mode.
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = "Mover|Movement Modes|Continuouss", Abstract)
class MODULARMOVER_API UStdContinuousOnGroundMove : public UBaseContinuousMove
{
	GENERATED_BODY()
public:
	UStdContinuousOnGroundMove();

	// Ground object type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	TArray<TEnumAsByte<ECollisionChannel>> GroundChannels = TArray<TEnumAsByte<ECollisionChannel>>{ECC_WorldStatic, ECC_WorldDynamic};

	// Maximum distance to detect surface below.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxGroundDistance = 50;

	// Maximum step height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxStepHeight = 50;

	// Maximum surface angle. Pass this value a surface won't even be considered walkable nor slidable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxSurfaceAngle = 70;

	// Maximum slope angle to start sliding (deg)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MaxSlopeAngle = 45;

	// Minimum slope angle to start sliding (deg)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Detection")
	float MinSlopeAngle = 35;


	// The name of the Root motion linear velocity input, as vector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	FName RootMotionLinearVelocityInput = NAME_None;

	// The name of the Root motion angular velocity input, as Rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	FName RootMotionAngularVelocityInput = NAME_None;

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

	// Enable down snapping to ground
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Ground | Movement")
	bool bDownSnapping = false;


	virtual bool CheckContinuousMovement_Implementation(UActorComponent* MoverActorComponent, const TArray<FMoverHitResult>& Surfaces, FContinuousMoveInfos& MoveInfos,
	                                                    const FMomentum& CurrentMomentum, const FVector MoveInput,
	                                                    const FMoverInputPool Inputs, const TArray<FContinuousMoveInfos>& ContinuousMoves, const TArray<FTemporaryMoveInfos>& TemporaryMoves,
	                                                    TMap<FName, FVector>& CustomProperties, int& SurfacesFlag) const override;

	virtual FMechanicProperties ProcessContinuousMovement_Implementation(UActorComponent* MoverActorComponent, FContinuousMoveInfos& MoveInfos, const FMomentum& CurrentMomentum,
	                                                                     const FVector MoveInput,
	                                                                     const FMoverInputPool Inputs, const FTransform SurfacesMovement, const float DeltaTime) const override;

	// Get the actual move params depending on inputs.
	FVector4 GetMoveParams(const FMoverInputPool& Inputs) const;
};
