// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "ModularMoverSubsystem.h"

#include <Kismet/KismetSystemLibrary.h>

#include "ModularMoverComponent.h"



void UModularMoverSubsystem::RegisterComponent(UModularMoverComponent* mover)
{
	if (!mover)
		return;
	if (_registeredMovers.Contains(mover))
		return;
	_registeredMovers.Add(mover);
	UpdateChunk(mover, mover->GetOwner()->GetActorTransform());
	mover->OnComponentMoved.AddUObject(this, &UModularMoverSubsystem::UpdateChunk);
}

void UModularMoverSubsystem::UnRegisterComponent(UModularMoverComponent* mover)
{
	if (!mover)
		return;
	const int index = _registeredMovers.IndexOfByKey(mover);
	if (!_registeredMovers.IsValidIndex(index))
		return;
	_registeredMovers.RemoveAt(index);
}

void UModularMoverSubsystem::UpdateChunk(UModularMoverComponent* mover, FTransform newTransform)
{
	if (!mover)
		return;
	const FVector location = newTransform.GetLocation();
	constexpr float divider = MOVER_CHUNK_SIZE;
	const int X = static_cast<int>(location.X / divider);
	const int Y = static_cast<int>(location.Y / divider);
	const int Z = static_cast<int>(location.Z / divider);
	const FChunkAreaID oldKey = mover->CurrentAreaChunk;
	const FChunkAreaID newKey = FChunkAreaID(X, Y, Z);
	if (oldKey == newKey)
	{
		if (!_areaChunks.Contains(oldKey))
		{
			FMoverAreaChunk chunk;
			chunk.Movers.Add(mover);
			_areaChunks.Add(oldKey, chunk);
		}
		return;
	}
	if (_areaChunks.Contains(oldKey))
	{
		const int oldIndex = _areaChunks[oldKey].Movers.IndexOfByKey(mover);
		if (_areaChunks[oldKey].Movers.IsValidIndex(oldIndex))
			_areaChunks[oldKey].Movers.RemoveAt(oldIndex);
	}

	mover->CurrentAreaChunk = newKey;

	if (!_areaChunks.Contains(newKey))
	{
		FMoverAreaChunk chunk;
		chunk.Movers.Add(mover);
		_areaChunks.Add(newKey, chunk);
		return;
	}

	const int newIndex = _areaChunks[newKey].Movers.IndexOfByKey(mover);
	if (!_areaChunks[newKey].Movers.IsValidIndex(newIndex))
	{
		_areaChunks[newKey].Movers.Add(mover);
	}
}

void UModularMoverSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UModularMoverSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UModularMoverSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TStatId UModularMoverSubsystem::GetStatId() const
{
	return GetStatID();
}

