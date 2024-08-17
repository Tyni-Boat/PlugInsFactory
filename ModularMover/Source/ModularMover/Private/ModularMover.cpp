// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModularMover.h"

#include "PhysicsEngine/PhysicsSettings.h"

#define LOCTEXT_NAMESPACE "FModularMoverModule"

void FModularMoverModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// UPhysicsSettings * PhysSetting = UPhysicsSettings::Get();
	// if(PhysSetting)
	// {
	// 	PhysSetting->bSubstepping = true;
	// 	PhysSetting->bSubsteppingAsync = true;
	// }
}

void FModularMoverModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModularMoverModule, ModularMover)