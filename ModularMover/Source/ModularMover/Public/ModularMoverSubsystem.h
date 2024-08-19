// Copyright Tyni Boat, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ModularMoverSubsystem.generated.h"

class UModularMoverComponent;


USTRUCT(BlueprintType)
struct FChunkAreaID
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoverChunk")
	int X = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoverChunk")
	int Y = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoverChunk")
	int Z = 0;

	FORCEINLINE bool Equals(const FChunkAreaID& other) const { return other.X == X && other.Y == Y && other.Z == Z; }

	FORCEINLINE bool operator==(const FChunkAreaID& other) const { return Equals(other); }
	
	FORCEINLINE bool operator!=(const FChunkAreaID& other) const { return !Equals(other); }
};


USTRUCT(BlueprintType)
struct FMoverAreaChunk
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoverChunk")
	TArray<UModularMoverComponent*> Movers;
};


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

public:

	void RegisterComponent(UModularMoverComponent* mover);

	void UpdateChunk(UModularMoverComponent* mover, FVector newLocation);
	
};
