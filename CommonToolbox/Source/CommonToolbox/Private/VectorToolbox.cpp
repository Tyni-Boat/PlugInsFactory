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

FVector UVectorToolbox::Project3DVector(FVector inVector, FVector planeNormal)
{
	if(!planeNormal.Normalize())
		return inVector;
	const float lenght = inVector.Length();
	const FVector result = FVector::VectorPlaneProject(inVector, planeNormal).GetSafeNormal() * lenght;
	return result;
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

bool UVectorToolbox::IntersectLineLine(const FVector& A1, const FVector& A2, const FVector& B1, const FVector& B2, FVector& IntersectionPoint)
{
	const FVector da = A2 - A1;
	const FVector db = B2 - B1;
	const FVector dc = B1 - A1;

	const float crossDaDb = FVector::CrossProduct(da, db).Size();
	// Lines are parallel
	if (crossDaDb == 0)
		return false;

	const float s = FVector::CrossProduct(dc, db).Size() / crossDaDb;
	IntersectionPoint = A1 + s * da;
	return true;
}

int UVectorToolbox::IntersectLineSphere(const FVector& LinePoint, const FVector& LineDir, const FVector& SphereCenter, float SphereRadius, FVector& IntersectionPoint1,
	FVector& IntersectionPoint2)
{
	FVector oc = LinePoint - SphereCenter;
	const float a = FVector::DotProduct(LineDir, LineDir);
	const float b = 2.0f * FVector::DotProduct(oc, LineDir);
	const float c = FVector::DotProduct(oc, oc) - SphereRadius * SphereRadius;
	const float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		// No intersection
		return 0;
	} else if (discriminant == 0) {
		// One intersection (tangent)
		float t = -b / (2 * a);
		IntersectionPoint1 = LinePoint + t * LineDir;
		IntersectionPoint2 = IntersectionPoint1;
		return 1;
	} else {
		// Two intersections (secant)
		const float sqrtDiscriminant = FMath::Sqrt(discriminant);
		const float t1 = (-b + sqrtDiscriminant) / (2 * a);
		const float t2 = (-b - sqrtDiscriminant) / (2 * a);

		IntersectionPoint2 = LinePoint + t1 * LineDir;
		IntersectionPoint1 = LinePoint + t2 * LineDir;
		return 2;
	}
}

bool UVectorToolbox::IntersectLineBox(const FBox& Box, const FTransform& BoxTransform, const FVector& LineStart, const FVector& LineEnd, FVector& OutIntersection)
{
	const FVector start = BoxTransform.InverseTransformPosition(LineStart);
	const FVector end = BoxTransform.InverseTransformPosition(LineEnd);
	FVector HitLocation, HitNormal;
	float HitTime = 0;
	if (FMath::LineExtentBoxIntersection(Box, start, end, FVector::ZeroVector, HitLocation, HitNormal, HitTime))
	{
		HitLocation = BoxTransform.TransformVector(HitLocation);
		OutIntersection = HitLocation;
		return true;
	}

	return false;
}
