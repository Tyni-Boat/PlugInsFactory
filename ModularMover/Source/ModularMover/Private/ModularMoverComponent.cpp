// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "ModularMoverComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UModularMoverComponent::UModularMoverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoRegisterUpdatedComponent = true;
	SetTickGroup(TG_PostPhysics);
	OnCalculateCustomPhysics.BindUObject(this, &UModularMoverComponent::PhysicsTick);
	// ...
}


// Called when the game starts
void UModularMoverComponent::BeginPlay()
{
	Super::BeginPlay();
	if(!UpdatedComponent)
		UpdatedComponent = GetOwner()->GetRootComponent();
	if(UpdatedComponent)
		UpdatedPrimitive = Cast<UPrimitiveComponent>(UpdatedComponent);
	if(UpdatedPrimitive)
	{
		UpdatedPrimitive->SetSimulatePhysics(true);
		UpdatedPrimitive->bIgnoreRadialForce = true;
		UpdatedPrimitive->bIgnoreRadialImpulse = true;
		UpdatedPrimitive->SetEnableGravity(false);
		UpdatedPrimitive->SetUpdateKinematicFromSimulation(true);
	}
	// ...
}


// Called every frame
void UModularMoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Add custom physics on RootComponent's BodyInstance
	if (UpdatedPrimitive && UpdatedPrimitive->GetBodyInstance() != nullptr)
	{
		UpdatedPrimitive->GetBodyInstance()->AddCustomPhysics(OnCalculateCustomPhysics);
	}

	// ...
}

void UModularMoverComponent::PhysicsTick(float DeltaTime, FBodyInstance* BodyInstance)
{
	UKismetSystemLibrary::PrintString(this);
}
