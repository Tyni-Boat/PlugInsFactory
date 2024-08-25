// Copyright Tyni Boat, Inc. All Rights Reserved.
#include "DebugToolbox.h"
#include "Kismet/KismetSystemLibrary.h"

void UDebugToolbox::DrawDebugCircleOnHit(const FHitResult MyStructRef, float radius, FLinearColor color, float duration, float thickness, bool showImpactAxis)
{
	if (!MyStructRef.Component.IsValid())
		return;
	FVector up = MyStructRef.Normal;
	FVector axisUp = MyStructRef.ImpactNormal;
	if (!up.Normalize())
		return;
	const FVector hitPoint = MyStructRef.ImpactPoint + up * 0.01;
	FVector right = axisUp.Rotation().Quaternion().GetAxisY();
	FVector forward = FVector::CrossProduct(right, axisUp);
	if (showImpactAxis && axisUp.Normalize())
	{
		FVector::CreateOrthonormalBasis(forward, right, axisUp);
		UKismetSystemLibrary::DrawDebugArrow(MyStructRef.GetComponent(), hitPoint, hitPoint + axisUp * radius, (radius * 0.25), FColor::Blue, duration, thickness);
		UKismetSystemLibrary::DrawDebugArrow(MyStructRef.GetComponent(), hitPoint, hitPoint + forward * (radius * 0.5), (radius * 0.25), FColor::Red, duration, thickness);
		UKismetSystemLibrary::DrawDebugArrow(MyStructRef.GetComponent(), hitPoint, hitPoint + right * (radius * 0.5), (radius * 0.25), FColor::Green, duration, thickness);
	}
	right = up.Rotation().Quaternion().GetAxisY();
	forward = FVector::CrossProduct(right, up);
	FVector::CreateOrthonormalBasis(forward, right, up);
	UKismetSystemLibrary::DrawDebugCircle(MyStructRef.GetComponent(), hitPoint, radius, 32,
										  color, duration, thickness, right, forward);
}
