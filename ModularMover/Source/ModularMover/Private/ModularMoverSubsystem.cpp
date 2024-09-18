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
	FMomentum fakeMomentum;
	fakeMomentum.Transform = mover->GetOwner()->GetActorTransform();
	UpdateChunk(mover, FMoverCheckRequest(fakeMomentum, FMoverInputPool()));
	mover->OnComponentMoved.AddUObject(this, &UModularMoverSubsystem::UpdateChunk);
	for (int i = 0; i < mover->ContingentMoveClasses.Num(); i++)
	{
		mover->AddContingentMoveMode(mover->ContingentMoveClasses[i]);
	}
	for (int i = 0; i < mover->TransientMoveClasses.Num(); i++)
	{
		mover->AddTransientMoveMode(mover->TransientMoveClasses[i]);
	}
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
		mover->RemoveContingentMoveMode(mover->ContingentMoveState[i].BaseInfos.ModeName);
	for (int i = 0; i < mover->TransientMoveState.Num(); i++)
		mover->RemoveTransientMoveMode(mover->TransientMoveState[i].BaseInfos.ModeName);
}


void UModularMoverSubsystem::UpdateChunk(UModularMoverComponent* mover, FMoverCheckRequest Request)
{
	if (!mover)
		return;
	const FVector location = Request.Momentum.Transform.GetLocation();
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

bool UModularMoverSubsystem::AddContingentLibrary(FContingentMoveInfos& OutMoveInfos, TSubclassOf<UBaseContingentMove> ModeClass)
{
	if (!ModeClass)
		return false;
	if (const auto instance = Cast<UBaseContingentMove>(ModeClass->GetDefaultObject()))
	{
		if (instance->ModeName.IsNone())
			return false;
		if (_contigentModeLibrary.Contains(instance->ModeName))
		{
			_contigentModeLibrary[instance->ModeName].ReferenceCount++;
		}
		else
		{
			const FMoveModeReference reference = FMoveModeReference(instance);
			_contigentModeLibrary.Add(instance->ModeName, reference);
		}
		
		OutMoveInfos = FContingentMoveInfos(instance);
		return true;
	}
	
	return false;
}


bool UModularMoverSubsystem::RemoveContingentLibrary(FName ModeName)
{
	if (_contigentModeLibrary.Contains(ModeName))
	{
		_contigentModeLibrary[ModeName].ReferenceCount--;
		if (_contigentModeLibrary[ModeName].ReferenceCount <= 0)
		{
			_contigentModeLibrary.Remove(ModeName);
			return true;
		}
	}

	return false;
}

bool UModularMoverSubsystem::AddTransientLibrary(FTransientMoveInfos& OutMoveInfos, TSubclassOf<UBaseTransientMove> ModeClass)
{
	if (!ModeClass)
		return false;
	if (const auto instance = Cast<UBaseTransientMove>(ModeClass->GetDefaultObject()))
	{
		if (instance->ModeName.IsNone())
			return false;
		if (_transientModeLibrary.Contains(instance->ModeName))
		{
			_transientModeLibrary[instance->ModeName].ReferenceCount++;
		}
		else
		{
			const FMoveModeReference reference = FMoveModeReference(instance);
			_transientModeLibrary.Add(instance->ModeName, reference);
		}
		
		OutMoveInfos = FTransientMoveInfos(instance);
		return true;
	}
	
	return false;
}

bool UModularMoverSubsystem::RemoveTransientLibrary(FName ModeName)
{
	if (_transientModeLibrary.Contains(ModeName))
	{
		_transientModeLibrary[ModeName].ReferenceCount--;
		if (_transientModeLibrary[ModeName].ReferenceCount <= 0)
		{
			_transientModeLibrary.Remove(ModeName);
			return true;
		}
	}

	return false;
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

UBaseContingentMove* UModularMoverSubsystem::GetContingentMoveObject(const FName ModeName)
{
	if (!_contigentModeLibrary.Contains(ModeName))
		return {};
	return static_cast<UBaseContingentMove*>(_contigentModeLibrary[ModeName].MovementModeInstance.Get());
}

UBaseTransientMove* UModularMoverSubsystem::GetTransientMoveObject(const FName ModeName)
{
	if (!_transientModeLibrary.Contains(ModeName))
		return {};
	return static_cast<UBaseTransientMove*>(_transientModeLibrary[ModeName].MovementModeInstance.Get());
}
