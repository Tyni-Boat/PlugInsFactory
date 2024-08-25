// Copyright Tyni Boat, Inc. All Rights Reserved.
#include "VectorToolbox.h"



FVector UVectorToolbox::VectorCone(const FVector inVector, const FVector normal, const float alphaAngle)
{
	FVector n = normal;
	if (!n.Normalize())
		return inVector;
	const float angle = FMath::Clamp(alphaAngle, 0, 180);
	if (angle == 90)
	{
		if ((inVector | n) <= 0)
			return FVector::VectorPlaneProject(inVector, n);
		return inVector;
	}
	FVector v = inVector;
	if (!v.Normalize())
		return inVector;
	const float vectorLenght = inVector.Length();
	const float cosine = FMath::Cos(FMath::DegreesToRadians(angle));
	const float sine = FMath::Sin(FMath::DegreesToRadians(angle));
	FVector cosineVector = v.ProjectOnToNormal(n);
	FVector sineVector = FVector::VectorPlaneProject(v, n);
	if (angle < 90)
	{
		cosineVector = cosineVector.GetSafeNormal() * FMath::Clamp(cosineVector.Length(), FMath::Abs(cosine), 1);
		sineVector = sineVector.GetSafeNormal() * FMath::Clamp(sineVector.Length(), 0, FMath::Abs(sine));
	}
	else
	{
		cosineVector = cosineVector.GetSafeNormal() * FMath::Clamp(cosineVector.Length(), 0, FMath::Abs(cosine));
		sineVector = sineVector.GetSafeNormal() * FMath::Clamp(sineVector.Length(), FMath::Abs(sine), 1);
	}

	return (sineVector + cosineVector).GetSafeNormal() * vectorLenght;
}

bool UVectorToolbox::IsVectorCone(const FVector inVector, const FVector normal, const float alphaAngle)
{
	FVector n = normal;
	if (!n.Normalize())
		return false;
	const float angle = FMath::Clamp(alphaAngle, 0, 180);
	const float cosine = (inVector.GetSafeNormal() | n);
	if (FMath::RadiansToDegrees(FMath::Acos(cosine)) <= angle)
		return true;
	return false;
}

FVector UVectorToolbox::Project2DInputs(FVector2D input, FTransform transformRelative, FVector planeNormal)
{
	FVector direction = FVector(0);
	FVector fwd = transformRelative.GetRotation().GetForwardVector();
	FVector rht = transformRelative.GetRotation().GetRightVector();
	if (planeNormal.Length() > 0 && planeNormal.Normalize())
	{
		fwd = FVector::VectorPlaneProject(fwd, planeNormal).GetSafeNormal();
		rht = FVector::VectorPlaneProject(rht, planeNormal).GetSafeNormal();
	}
	const FVector compositeRhs = rht * input.X;
	const FVector compositeFwd = fwd * input.Y;
	direction = compositeFwd + compositeRhs;
	return direction;
}

FVector UVectorToolbox::GetSnapOnSurfaceVector(const FVector point, const FHitResult surface, const FVector axis)
{
	if (!surface.Component.IsValid())
		return FVector(0);
	const FVector snapDirection = (-axis).GetSafeNormal();
	const FVector hitPoint = surface.ImpactPoint;
	FVector snapVector = hitPoint - point;
	if (snapDirection.SquaredLength() > 0)
		snapVector = snapVector.ProjectOnToNormal(snapDirection);
	return snapVector;
}
