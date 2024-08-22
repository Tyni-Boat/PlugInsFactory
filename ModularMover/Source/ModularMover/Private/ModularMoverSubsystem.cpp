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
	for (int i = 0; i < mover->ContingentMoveClasses.Num(); i++)
		UpdateContingentLibrary("", false, mover->ContingentMoveClasses[i], &mover->ContingentMoveState);
	for (int i = 0; i < mover->TransientMoveClasses.Num(); i++)
		UpdateTransientLibrary("", false, mover->TransientMoveClasses[i], &mover->TransientMoveState);
}

void UModularMoverSubsystem::UnRegisterComponent(UModularMoverComponent* mover)
{
	if (!mover)
		return;
	const int index = _registeredMovers.IndexOfByKey(mover);
	if (!_registeredMovers.IsValidIndex(index))
		return;
	_registeredMovers.RemoveAt(index);
	for (int i = 0; i < mover->ContingentMoveState.Num(); i++)
		UpdateContingentLibrary(mover->ContingentMoveState[i].BaseInfos.ModeName, true);
	for (int i = 0; i < mover->TransientMoveState.Num(); i++)
		UpdateTransientLibrary(mover->TransientMoveState[i].BaseInfos.ModeName, true);
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


void UModularMoverSubsystem::UpdateContingentLibrary(FName MoveName, bool remove, TSubclassOf<UBaseContingentMove> ModeClass, TArray<FContingentMoveInfos>* liveListInfos)
{
	if (remove)
	{
		if (_contigentModeLibrary.Contains(MoveName))
		{
			_contigentModeLibrary[MoveName].ReferenceCount--;
			if (_contigentModeLibrary[MoveName].ReferenceCount <= 0)
			{
				_contigentModeLibrary.Remove(MoveName);
				if (liveListInfos)
				{
					const int index = liveListInfos->IndexOfByPredicate([MoveName](const FContingentMoveInfos& infos)-> bool { return infos.BaseInfos.ModeName == MoveName; });
					if (liveListInfos->IsValidIndex(index))
					{
						liveListInfos->RemoveAt(index);
					}
				}
			}
		}
	}
	else
	{
		if (_contigentModeLibrary.Contains(MoveName))
		{
			_contigentModeLibrary[MoveName].ReferenceCount++;
		}
		else if (ModeClass)
		{
			if (const auto instance = Cast<UBaseContingentMove>(ModeClass->GetDefaultObject()))
			{
				if (instance->ModeName.IsNone())
					return;
				const FMoveModeReference reference = FMoveModeReference(instance);
				_contigentModeLibrary.Add(instance->ModeName, reference);
				if (liveListInfos)
				{
					const int index = liveListInfos->IndexOfByPredicate([instance](const FContingentMoveInfos& infos)-> bool { return infos.BaseInfos.ModeName == instance->ModeName; });
					if (!liveListInfos->IsValidIndex(index))
					{
						liveListInfos->Add(FContingentMoveInfos(instance));
					}
				}
			}
		}
	}
}


void UModularMoverSubsystem::UpdateTransientLibrary(FName MoveName, bool remove, TSubclassOf<UBaseTransientMove> ModeClass, TArray<FTransientMoveInfos>* liveListInfos)
{
	if (remove)
	{
		if (_transientModeLibrary.Contains(MoveName))
		{
			_transientModeLibrary[MoveName].ReferenceCount--;
			if (_transientModeLibrary[MoveName].ReferenceCount <= 0)
			{
				_transientModeLibrary.Remove(MoveName);
				if (liveListInfos)
				{
					const int index = liveListInfos->IndexOfByPredicate([MoveName](const FTransientMoveInfos& infos)-> bool { return infos.BaseInfos.ModeName == MoveName; });
					if (liveListInfos->IsValidIndex(index))
					{
						liveListInfos->RemoveAt(index);
					}
				}
			}
		}
	}
	else
	{
		if (_transientModeLibrary.Contains(MoveName))
		{
			_transientModeLibrary[MoveName].ReferenceCount++;
		}
		else if (ModeClass)
		{
			if (const auto instance = Cast<UBaseTransientMove>(ModeClass->GetDefaultObject()))
			{
				if (instance->ModeName.IsNone())
					return;
				const FMoveModeReference reference = FMoveModeReference(instance);
				_transientModeLibrary.Add(instance->ModeName, reference);
				if (liveListInfos)
				{
					const int index = liveListInfos->IndexOfByPredicate([instance](const FTransientMoveInfos& infos)-> bool { return infos.BaseInfos.ModeName == instance->ModeName; });
					if (!liveListInfos->IsValidIndex(index))
					{
						liveListInfos->Add(FTransientMoveInfos(instance));
					}
				}
			}
		}
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
