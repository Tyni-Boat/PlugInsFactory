// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "ModularMoverSubsystem.h"

#include <Kismet/KismetSystemLibrary.h>

#include "ModularMoverComponent.h"


bool UModularMoverSubsystem::AddContinuousLibrary(FContinuousMoveInfos& OutMoveInfos, TSubclassOf<UBaseContinuousMove> ModeClass)
{
	if (!ModeClass)
		return false;
	if (const auto instance = Cast<UBaseContinuousMove>(ModeClass->GetDefaultObject()))
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

		OutMoveInfos = FContinuousMoveInfos(instance);
		return true;
	}

	return false;
}

bool UModularMoverSubsystem::RemoveContinuousLibrary(FName ModeName)
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

bool UModularMoverSubsystem::AddTemporaryLibrary(FTemporaryMoveInfos& OutMoveInfos, TSubclassOf<UBaseTemporaryMove> ModeClass)
{
	if (!ModeClass)
		return false;
	if (const auto instance = Cast<UBaseTemporaryMove>(ModeClass->GetDefaultObject()))
	{
		if (instance->ModeName.IsNone())
			return false;
		if (_TemporaryModeLibrary.Contains(instance->ModeName))
		{
			_TemporaryModeLibrary[instance->ModeName].ReferenceCount++;
		}
		else
		{
			const FMoveModeReference reference = FMoveModeReference(instance);
			_TemporaryModeLibrary.Add(instance->ModeName, reference);
		}

		OutMoveInfos = FTemporaryMoveInfos(instance);
		return true;
	}

	return false;
}

bool UModularMoverSubsystem::RemoveTemporaryLibrary(FName ModeName)
{
	if (_TemporaryModeLibrary.Contains(ModeName))
	{
		_TemporaryModeLibrary[ModeName].ReferenceCount--;
		if (_TemporaryModeLibrary[ModeName].ReferenceCount <= 0)
		{
			_TemporaryModeLibrary.Remove(ModeName);
			return true;
		}
	}

	return false;
}



void UModularMoverSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	_customPhysicMaterial = NewObject<UPhysicalMaterial>();
	_customPhysicMaterial->bOverrideFrictionCombineMode = true;
	_customPhysicMaterial->bOverrideRestitutionCombineMode = true;
	_customPhysicMaterial->Friction = 0;
	_customPhysicMaterial->StaticFriction = 0;
	_customPhysicMaterial->FrictionCombineMode = EFrictionCombineMode::Min;
	_customPhysicMaterial->Restitution = 0;
	_customPhysicMaterial->RestitutionCombineMode = EFrictionCombineMode::Min;
}

void UModularMoverSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UModularMoverSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateTrackedSurface(DeltaTime);
}

TStatId UModularMoverSubsystem::GetStatId() const
{
	return GetStatID();
}



UBaseContinuousMove* UModularMoverSubsystem::GetContinuousMoveObject(const FName ModeName)
{
	if (!_contigentModeLibrary.Contains(ModeName))
		return {};
	return static_cast<UBaseContinuousMove*>(_contigentModeLibrary[ModeName].MovementModeInstance.Get());
}

UBaseTemporaryMove* UModularMoverSubsystem::GetTemporaryMoveObject(const FName ModeName)
{
	if (!_TemporaryModeLibrary.Contains(ModeName))
		return {};
	return static_cast<UBaseTemporaryMove*>(_TemporaryModeLibrary[ModeName].MovementModeInstance.Get());
}

void UModularMoverSubsystem::AddTrackedSurface(const FHitResult& hit)
{
	if (!hit.bBlockingHit && !hit.bStartPenetrating)
		return;
	if (_trackedSurfaces.Contains(hit.Component))
		return;
	_trackedSurfaces.Add(hit.Component, FSurfaceVelocity(hit));
	hit.Component->OnComponentDeactivated.AddDynamic(this, &UModularMoverSubsystem::RemoveTrackedComponent);
}



UPhysicalMaterial* UModularMoverSubsystem::GetStdPhysicMaterial() const
{
	return _customPhysicMaterial;
}

FSurfaceVelocity UModularMoverSubsystem::GetSurfaceVelocity(const TWeakObjectPtr<UPrimitiveComponent> target) const
{
	return _trackedSurfaces.Contains(target) ? _trackedSurfaces[target] : FSurfaceVelocity();
}

FSurfaceVelocity UModularMoverSubsystem::GetSurfaceVelocity(const FSurface surface) const
{
	return _trackedSurfaces.Contains(surface.HitResult.Component) ? _trackedSurfaces[surface.HitResult.Component] : FSurfaceVelocity();
}

TArray<FSurfaceVelocity> UModularMoverSubsystem::GetSurfaceVelocities(const TArray<TWeakObjectPtr<UPrimitiveComponent>>& targets) const
{
	TArray<FSurfaceVelocity> surfMobs;
	for (const auto t : targets)
		surfMobs.Add(GetSurfaceVelocity(t));
	return surfMobs;
}

TArray<FSurfaceVelocity> UModularMoverSubsystem::GetSurfaceVelocities(const TArray<FSurface>& surfaces) const
{
	TArray<FSurfaceVelocity> surfMobs;
	for (const auto t : surfaces)
		surfMobs.Add(GetSurfaceVelocity(t));
	return surfMobs;
}



void UModularMoverSubsystem::UpdateTrackedSurface(const float& deltaTime)
{
	for (auto SurfaceMob : _trackedSurfaces)
	{
		_trackedSurfaces[SurfaceMob.Key].UpdateTracking(deltaTime);
	}
}

void UModularMoverSubsystem::RemoveTrackedComponent(UActorComponent* Component)
{
	if (!Component)
		return;
	const UPrimitiveComponent* asPrimitive = Cast<UPrimitiveComponent>(Component);
	if (!asPrimitive)
		return;
	if (!_trackedSurfaces.Contains(asPrimitive))
		return;
	_trackedSurfaces.Remove(asPrimitive);
}
