// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "TypesLibrary.h"
#include "Subsystems/WorldSubsystem.h"
#include "ModularMoverSubsystem.generated.h"

class UModularMoverComponent;

#define MOVER_CHUNK_SIZE 1000.0f


/**
 * 
 */
UCLASS()
class MODULARMOVER_API UModularMoverSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<UModularMoverComponent*> _registeredMovers;

	UPROPERTY()
	TMap<FChunkAreaID, FMoverAreaChunk> _areaChunks;

	UPROPERTY()
	TMap<FName, FMoveModeReference> _contigentModeLibrary;

	UPROPERTY()
	TMap<FName, FMoveModeReference> _transientModeLibrary;

public:
	void RegisterComponent(UModularMoverComponent* mover);

	void UnRegisterComponent(UModularMoverComponent* mover);

	void UpdateChunk(UModularMoverComponent* mover, FTransform newTransform);

	bool AddContingentLibrary(FContingentMoveInfos& OutMoveInfos, TSubclassOf<UBaseContingentMove> ModeClass = nullptr);
	
	bool RemoveContingentLibrary(FName ModeName);

	bool AddTransientLibrary(FTransientMoveInfos& OutMoveInfos, TSubclassOf<UBaseTransientMove> ModeClass = nullptr);
	
	bool RemoveTransientLibrary(FName ModeName);

	void Initialize(FSubsystemCollectionBase& Collection) override;

	void Deinitialize() override;

	void Tick(float DeltaTime) override;

	TStatId GetStatId() const override;

	TSoftObjectPtr<UBaseMoverMovementMode> GetContingentMoveObject(const FName ModeName);

	TSoftObjectPtr<UBaseMoverMovementMode> GetTransientMoveObject(const FName ModeName);
};
