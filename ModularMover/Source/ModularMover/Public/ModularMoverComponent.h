// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "CommonToolboxTypeLibrary.h"
#include "ModularMoverSubsystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicsPublic.h"
#include "TypesLibrary.h"
#include "Components/ActorComponent.h"
#include "GameFramework/MovementComponent.h"
#include "ModularMoverComponent.generated.h"


#define OVERLAP_INFLATION 5
#define DRAG_To_DAMPING 2.4026


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

	~UModularMoverComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Register the component to it's subsystem.
	bool InitialRegistration();

	// Unregister the component to it's subsystem.
	bool FinalUnregistration();

	// The reference to the current subsystem.
	UModularMoverSubsystem* _subSystem = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	EDebugMode DebugMode = EDebugMode::None;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void AsyncPhysicsTickComponent(float DeltaTime, float SimTime) override;

	// Evaluate continuous and instantaneous movement modes as well as Traversals
	void EvaluateMovementSurface(const FTransform Transform, TArray<FExpandedHitResult> Surfaces);


#pragma region Physic

public:
	// Called when ever the location of the controller changed.
	FOnMoveDelegate OnComponentMoved;

	// The rotation offset. helpful when using rotated skeletal mesh components 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Physic|Spacial")
	FRotator RotationOffset;

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

	// Use asynchronous surface detection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Physic|Collisions")
	bool bUseAsyncSurfaceDetection = false;

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
	UPROPERTY()
	UPhysicalMaterial* _customPhysicMaterial = nullptr;
	FVector _lastLocation;
	FVector _lastLocation_trigger;
	FQuat _lastRotation;
	FQuat _lastRotation_trigger;
	FTraceDelegate _onMainSurfaceChk;
	TMap<FTraceHandle, FTransform> _chkRequestMap;


	UFUNCTION(BlueprintPure, Category="Mover|Physic")
	bool IsIgnoringCollision() const;

	UFUNCTION(BlueprintPure, Category="Mover|Physic")
	FVector GetCurrentScanSurfaceVector() const;

	UFUNCTION(BlueprintPure, Category="Mover|Physic")
	float GetCurrentScanSurfaceOffset() const;

	// Get the controller Mass
	UFUNCTION(BlueprintPure, Category = "Mover|Physic")
	FORCEINLINE float GetMass() const
	{
		return (Mass < 0 && UpdatedPrimitive != nullptr && UpdatedPrimitive->IsSimulatingPhysics()) ? UpdatedPrimitive->GetMass() : FMath::Clamp(Mass, 1, TNumericLimits<double>().Max());
	}

	// Check movement diff and trigger on move events
	void EvaluateMovementDiff(const FBodyInstance* body);

	// Run the solver when ever moved
	void OnMoveCheck(UModularMoverComponent* Mover, FTransform Transform);

	// Called when we've done the surface check trace async
	void OnMainSurfaceCheckDone(const FTraceHandle& TraceHandle, FTraceDatum& TraceData);

	//Solve overlap problems recursively.
	void OverlapSolver(int& maxDepth, const FTransform* customTr = nullptr) const;

	// Detect surface overlap hits
	bool DetectOverlapHits(const FTransform Transform, TArray<FExpandedHitResult>& touchedHits, const FVector scanVector = FVector(0), const uint64 CounterDirectionOffset = 0) const;

	// Detect surface overlap hits async
	FTraceHandle AsyncDetectOverlapHits(const FTransform Transform, FTraceDelegate* callBack, const FVector scanVector = FVector(0), const uint64 CounterDirectionOffset = 0) const;

	// De-penetrate overlaps
	void FixOverlapHits(int& maxDepth, const FTransform Transform, const TArray<FExpandedHitResult> touchedHits, std::function<void(FVector)> OnLocationSet = {},
	                    std::function<void(UPrimitiveComponent*, FVector)> OnPhysicCompHit = {}) const;

	// Calculate velocity. UseReduction subtract the current Linear Velocity from the end result.
	static FVector ComputeLinearVelocity(FLinearMechanic AttemptedMovement, FVector currentLinearVelocity, const float mass, const float deltaTime, bool UseReduction = false);

#pragma endregion

#pragma region Movement Modes

	// Contingents ------------------------------------------------------------------------------------------------------------------------------------------

public:
	// The contingent move modes used on this controller by default
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Mover|Movement Modes|Contingent")
	TArray<TSubclassOf<UBaseContingentMove>> ContingentMoveClasses;

	// The states of each contingent move for this mover
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, category = "Mover|Movement Modes|Contingent")
	TArray<FContingentMoveInfos> ContingentMoveState;

	// Transients ------------------------------------------------------------------------------------------------------------------------------------------

public:
	// The Transient move modes used on this controller by default
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Mover|Movement Modes|Transient")
	TArray<TSubclassOf<UBaseTransientMove>> TransientMoveClasses;

	// The states of each Transient move for this mover
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, category = "Mover|Movement Modes|Transient")
	TArray<FTransientMoveInfos> TransientMoveState;

	// Traversal ------------------------------------------------------------------------------------------------------------------------------------------

#pragma endregion

#pragma region Flow Mode

	// Stand alone mode

#pragma endregion

#pragma region Movement

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	//FMomentum BodyMomentum = FMomentum();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	FMechanicProperties DebugMovement;


	// Move a body according to Movement
	void MoveBody(FBodyInstance* Body, const FMechanicProperties movement, const float Delta);

	// Get the orientation, keeping body upright.
	bool GetAngularOrientation(FQuat& Orientation, const FBodyInstance* Body, const FAngularMechanic angularMechanic, const FVector Gravity, const float inDelta) const;

	// Calculate an Angular Velocity to fit orientation, keeping body upright.
	FVector GetAngularVelocity(const FBodyInstance* Body, const FAngularMechanic angularMechanic, const FVector Gravity, const float inDelta) const;

	// Get the trajectory
	UFUNCTION(BlueprintPure, Category="Mover|Movement")
	static TArray<FMomentum> GetTrajectory(const int SampleCount, const FMechanicProperties inputMovement, const FMomentum currentMomentum, const float timeStep);

#pragma endregion
};
