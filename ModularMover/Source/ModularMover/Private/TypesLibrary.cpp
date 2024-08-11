// Copyright Tyni Boat, Inc. All Rights Reserved.


#include "TypesLibrary.h"

#include "../../CommonToolbox/Source/CommonToolbox/Public/CommonToolboxBPLibrary.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"


#pragma region Enums
#pragma endregion

#pragma region Structs

#pragma endregion

#pragma region Classes

UBaseMoverState::UBaseMoverState()
{
}

void UBaseMoverState::TestTrace(FString message, FTransform tr, FCollisionShape shape) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("TestTrace");
	if (!GetWorld())
		return;
	FHitResult hitImpact;
	bool hit = GetWorld()->SweepSingleByChannel(hitImpact, tr.GetLocation(), tr.GetLocation() + FVector::DownVector * 5000
	                                            , tr.GetRotation(), ECollisionChannel::ECC_Visibility, shape);
	if (hit)
		// UCommonToolboxBPLibrary::DrawDebugCircleOnHit(hitImpact, 40, FColor::Red, 0, 1, true);
		UCommonToolboxBPLibrary::TenPowX(10);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Debugging %s: message(%s), priority(%d)"), *StateName.ToString(), *message, Priority)
	                                  , true, false, FColor::Orange, 0, StateName);
}

bool UBaseMoverState::IsValid() const
{
	return StateName != NAME_None && Priority >= 0;
}

#pragma endregion
