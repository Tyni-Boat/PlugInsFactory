// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "ModularMoverSubsystem.h"
#include "PhysicsPublic.h"
#include "TypesLibrary.h"
#include "Components/ActorComponent.h"
#include "GameFramework/MovementComponent.h"
#include "ModularMoverComponent.generated.h"


#define OVERLAP_INFLATION 5


class UModularMoverComponent;


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMoveDelegate, UModularMoverComponent*, FTransform)


UCLASS(ClassGroup = "Controllers",
	hidecategories = (Sockets, Object, LOD, Lighting, TextureStreaming, Velocity
		, PlanarMovement, ComponentTick, ComponentReplication, MovementComponent
		, Tags, Replication, Navigation, Activation, Cooking, AssetUserData, Collision),
	meta = (DisplayName = "Modular Controller", BlueprintSpawnableComponent))
class MODULARMOVER_API UModularMoverComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UModularMoverComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Register the component to it's subsystem.
	bool InitialRegistration();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	EDebugMode DebugMode = EDebugMode::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TSubclassOf<UBaseMoverMovementMode> stateClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default", Transient)
	UBaseMoverMovementMode* state;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void AsyncPhysicsTickComponent(float DeltaTime, float SimTime) override;


#pragma region Physic

public:
	// Called when ever the location of the controller changed.
	FOnMoveDelegate OnComponentMoved;

	// The current area chunk
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Physic|Spacial")
	FChunkAreaID CurrentAreaChunk;

	// The distance tolerance to trigger the component moved event
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Physic|Spacial")
	float MoveTriggerTolerance = 0.1;

	// The angular tolerance to trigger the component moved event. (degrees)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Physic|Spacial")
	float RotateTriggerTolerance = 15;

	// Disable collision with the world and other characters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	bool bDisableCollision = false;

	// The list of components to ignore when moving.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	TArray<UPrimitiveComponent*> IgnoredCollisionComponents;

	// Use complex collision for movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	bool bUseComplexCollision = false;

	// The minimum depth when detecting depth surface
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	float MinDepthSurface = 10;

	// The component mass
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|RigidBody")
	float Mass = 80;

protected:
	FVector _lastLocation;
	FVector _lastLocation_trigger;
	FQuat _lastRotation;
	FQuat _lastRotation_trigger;


	UFUNCTION(BlueprintPure, Category="Mover|Physic")
	bool IsIgnoringCollision() const;

	// Get the controller Mass
	UFUNCTION(BlueprintPure, Category = "Controllers|Physic")
	FORCEINLINE float GetMass() const
	{
		return (Mass < 0 && UpdatedPrimitive != nullptr && UpdatedPrimitive->IsSimulatingPhysics()) ? UpdatedPrimitive->GetMass() : FMath::Clamp(Mass, 1, TNumericLimits<double>().Max());
	}

	// Check movement diff and trigger on move events
	void EvaluateMovementDiff(const FBodyInstance* body);

	// Run the solver when ever moved
	void OnMoveCheck(UModularMoverComponent* Mover, FTransform Transform);

	//Solve overlap problems recursively.
	void OverlapSolver(int& maxDepth, const FTransform* customTr = nullptr) const;

	// Detect surface overlap hits
	bool DetectOverlapHits(const FTransform Transform, TArray<FExpandedHitResult>& touchedHits, const FVector scanVector = FVector(0), const uint64 CounterDirectionOffset = 0) const;

	// De-penetrate overlaps
	void FixOverlapHits(int& maxDepth, const FTransform Transform, const TArray<FExpandedHitResult> touchedHits, std::function<void(FVector)> OnLocationSet = {},
	                    std::function<void(UPrimitiveComponent*, FVector)> OnPhysicCompHit = {}) const;

#pragma endregion

#pragma region Flow Mode

	// Stand alone mode

#pragma endregion
};
