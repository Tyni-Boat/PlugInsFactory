// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "TypesLibrary.h"
#include "Subsystems/WorldSubsystem.h"
#include "ModularMoverSubsystem.generated.h"

/**
 * Modular Mover SubSystem keeping reference of movement mode instances.
 */
UCLASS()
class MODULARMOVER_API UModularMoverSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TMap<FName, FMoveModeReference> _contigentModeLibrary;

	UPROPERTY()
	TMap<FName, FMoveModeReference> _TemporaryModeLibrary;

	UPROPERTY()
	TMap<TWeakObjectPtr<UPrimitiveComponent>, FSurfaceVelocity> _trackedSurfaces;

	UPROPERTY()
	UPhysicalMaterial* _customPhysicMaterial = nullptr;

public:

	// Try Create/Add a continuous move class.
	bool AddContinuousLibrary(FContinuousMoveInfos& OutMoveInfos, TSubclassOf<UBaseContinuousMove> ModeClass = nullptr);

	// Try remove a continuous move class.
	bool RemoveContinuousLibrary(FName ModeName);

	// Try Create/Add a Temporary move class.
	bool AddTemporaryLibrary(FTemporaryMoveInfos& OutMoveInfos, TSubclassOf<UBaseTemporaryMove> ModeClass = nullptr);

	// Try remove a Temporary move class.
	bool RemoveTemporaryLibrary(FName ModeName);


	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void Deinitialize() override;
	
	void Tick(float DeltaTime) override;

	TStatId GetStatId() const override;
	

	// Get continuous object instance of name.
	UBaseContinuousMove* GetContinuousMoveObject(const FName ModeName);

	// Get temporary object instance of name.
	UBaseTemporaryMove* GetTemporaryMoveObject(const FName ModeName);

	// Register a surface to be tracked.
	void AddTrackedSurface(const FHitResult& hit);

	// Get the zero friction, zero bounciness material instance
	UPhysicalMaterial* GetStdPhysicMaterial() const;

	FSurfaceVelocity GetSurfaceVelocity(const TWeakObjectPtr<UPrimitiveComponent> target) const;

	FSurfaceVelocity GetSurfaceVelocity(const FSurface surface) const;

	TArray<FSurfaceVelocity> GetSurfaceVelocities(const TArray<TWeakObjectPtr<UPrimitiveComponent>>& targets) const;

	TArray<FSurfaceVelocity> GetSurfaceVelocities(const TArray<FSurface>& surfaces) const;

private:
	
	void UpdateTrackedSurface(const float& deltaTime);

	void RemoveTrackedComponent(UActorComponent* Component);
};

