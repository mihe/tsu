#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "TsuVectorLibrary.generated.h"

UCLASS(ClassGroup=TSU)
class UTsuVectorLibrary final
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Calculate the cross product of two vectors.
	 *
	 * @param Other The other vector.
	 * @return The cross product.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Cross(FVector Vector, FVector Other)
	{
		return Vector ^ Other;
	}

	/**
	 * Calculate the dot product between this and another vector.
	 *
	 * @param Other The other vector.
	 * @return The dot product.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float Dot(FVector Vector, FVector Other)
	{
		return Vector | Other;
	}

	/**
	 * Gets the result of component-wise addition of this and another vector.
	 *
	 * @param Other The vector to add to this.
	 * @return The result of vector addition.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Add(FVector Vector, FVector Other)
	{
		return Vector + Other;
	}

	/**
	 * Gets the result of adding to each component of the vector.
	 *
	 * @param Bias How much to add to each component.
	 * @return The result of addition.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector AddFloat(FVector Vector, float Bias)
	{
		return Vector + Bias;
	}

	/**
	 * Gets the result of component-wise subtraction of this by another vector.
	 *
	 * @param Other The vector to subtract from this.
	 * @return The result of vector subtraction.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Subtract(FVector Vector, FVector Other)
	{
		return Vector - Other;
	}

	/**
	 * Gets the result of subtracting from each component of the vector.
	 *
	 * @param Bias How much to subtract from each component.
	 * @return The result of subtraction.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector SubtractFloat(FVector Vector, float Bias)
	{
		return Vector - Bias;
	}

	/**
	 * Gets the result of component-wise division of this vector by another.
	 *
	 * @param Other The vector to divide by.
	 * @return The result of division.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Divide(FVector Vector, FVector Other)
	{
		return Vector / Other;
	}

	/**
	 * Gets the result of dividing each component of the vector by a value.
	 *
	 * @param Scale What to divide each component by.
	 * @return The result of division.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector DivideFloat(FVector Vector, float Scale)
	{
		return Vector / Scale;
	}

	/**
	 * Gets the result of component-wise multiplication of this vector by another.
	 *
	 * @param Other The vector to multiply with.
	 * @return The result of multiplication.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Multiply(FVector Vector, FVector Other)
	{
		return Vector * Other;
	}

	/**
	 * Gets the result of scaling the vector (multiplying each component by a value).
	 *
	 * @param Scale What to multiply each component by.
	 * @return The result of multiplication.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector MultiplyFloat(FVector Vector, float Scale)
	{
		return Vector * Scale;
	}

	/**
	 * Check against another vector for equality, within specified error limits.
	 *
	 * @param Other The vector to check against.
	 * @param Tolerance Error tolerance.
	 * @return true if the vectors are equal within tolerance limits, false otherwise.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool Equals(FVector Vector, FVector Other, float Tolerance = 0.0001f)
	{
		return Vector.Equals(Other, Tolerance);
	}

	/**
	 * Checks whether all components of this vector are the same, within a tolerance.
	 *
	 * @param Tolerance Error tolerance.
	 * @return true if the vectors are equal within tolerance limits, false otherwise.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool AllComponentsEqual(FVector Vector, float Tolerance = 0.0001f)
	{
		return Vector.AllComponentsEqual(Tolerance);
	}

	/**
	 * Get a negated copy of the vector.
	 *
	 * @return A negated copy of the vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Negate(FVector Vector)
	{
		return -Vector;
	}

	/**
	 * Gets specific component of the vector.
	 *
	 * @param Index The index of the component required.
	 * @return Copy of the specified component.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float Component(FVector Vector, int32 Index)
	{
		return Vector[Index];
	}

	/**
	 * Get the maximum value of the vector's components.
	 *
	 * @return The maximum value of the vector's components.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float GetMax(FVector Vector)
	{
		return Vector.GetMax();
	}

	/**
	 * Get the maximum absolute value of the vector's components.
	 *
	 * @return The maximum absolute value of the vector's components.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float GetAbsMax(FVector Vector)
	{
		return Vector.GetAbsMax();
	}

	/**
	 * Get the minimum value of the vector's components.
	 *
	 * @return The minimum value of the vector's components.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float GetMin(FVector Vector)
	{
		return Vector.GetMin();
	}

	/**
	 * Get the minimum absolute value of the vector's components.
	 *
	 * @return The minimum absolute value of the vector's components.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float GetAbsMin(FVector Vector)
	{
		return Vector.GetAbsMin();
	}

	/** Gets the component-wise min of two vectors. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector ComponentMin(FVector Vector, FVector Other)
	{
		return Vector.ComponentMin(Other);
	}

	/** Gets the component-wise max of two vectors. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector ComponentMax(FVector Vector, FVector Other)
	{
		return Vector.ComponentMax(Other);
	}

	/**
	 * Get a copy of this vector with absolute value of each component.
	 *
	 * @return A copy of this vector with absolute value of each component.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetAbs(FVector Vector)
	{
		return Vector.GetAbs();
	}

	/**
	 * Get the length (magnitude) of this vector.
	 *
	 * @return The length of this vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float Size(FVector Vector)
	{
		return Vector.Size();
	}

	/**
	 * Get the squared length of this vector.
	 *
	 * @return The squared length of this vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float SizeSquared(FVector Vector)
	{
		return Vector.Size2D();
	}

	/**
	 * Get the length of the 2D components of this vector.
	 *
	 * @return The 2D length of this vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float Size2D(FVector Vector)
	{
		return Vector.Size2D();
	}

	/**
	 * Get the squared length of the 2D components of this vector.
	 *
	 * @return The squared 2D length of this vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float SizeSquared2D(FVector Vector)
	{
		return Vector.SizeSquared2D();
	}

	/**
	 * Checks whether vector is near to zero within a specified tolerance.
	 *
	 * @param Tolerance Error tolerance.
	 * @return true if the vector is near to zero, false otherwise.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool IsNearlyZero(FVector Vector, float Tolerance = 0.0001f)
	{
		return Vector.IsNearlyZero(Tolerance);
	}

	/**
	 * Checks whether all components of the vector are exactly zero.
	 *
	 * @return true if the vector is exactly zero, false otherwise.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool IsZero(FVector Vector)
	{
		return Vector.IsZero();
	}

	/**
	 * Checks whether vector is normalized.
	 *
	 * @return true if Normalized, false otherwise.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool IsNormalized(FVector Vector)
	{
		return Vector.IsNormalized();
	}

	/**
	 * Util to convert this vector into a unit direction vector and its original length.
	 *
	 * @param OutDir Reference passed in to store unit direction vector.
	 * @param OutLength Reference passed in to store length of the vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static void ToDirectionAndLength(FVector Vector, FVector& OutDir, float& OutLength)
	{
		Vector.ToDirectionAndLength(OutDir, OutLength);
	}

	/**
	 * Get a copy of the vector as sign only.
	 * Each component is set to +1 or -1, with the sign of zero treated as +1.
	 *
	 * @param A copy of the vector with each component set to +1 or -1
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetSignVector(FVector Vector)
	{
		return Vector.GetSignVector();
	}

	/**
	 * Projects 2D components of vector based on Z.
	 *
	 * @return Projected version of vector based on Z.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Projection(FVector Vector)
	{
		return Vector.Projection();
	}

	/**
	 * Calculates normalized version of vector without checking for zero length.
	 *
	 * @return Normalized version of vector.
	 * @see GetSafeNormal()
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetUnsafeNormal(FVector Vector)
	{
		return Vector.GetUnsafeNormal();
	}

	/**
	 * Gets a copy of this vector snapped to a grid.
	 *
	 * @param GridSz Grid dimension.
	 * @return A copy of this vector snapped to a grid.
	 * @see FMath::GridSnap()
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GridSnap(FVector Vector, float GridSz)
	{
		return Vector.GridSnap(GridSz);
	}

	/**
	 * Get a copy of this vector, clamped inside of a cube.
	 *
	 * @param Radius Half size of the cube.
	 * @return A copy of this vector, bound by cube.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector BoundToCube(FVector Vector, float Radius)
	{
		return Vector.BoundToCube(Radius);
	}

	/** Create a copy of this vector, with its magnitude clamped between Min and Max. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetClampedToSize(FVector Vector, float Min, float Max)
	{
		return Vector.GetClampedToSize(Min, Max);
	}

	/** Create a copy of this vector, with the 2D magnitude clamped between Min and Max. Z is unchanged. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetClampedToSize2D(FVector Vector, float Min, float Max)
	{
		return Vector.GetClampedToSize2D(Min, Max);
	}

	/** Create a copy of this vector, with its maximum magnitude clamped to MaxSize. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetClampedToMaxSize(FVector Vector, float MaxSize)
	{
		return Vector.GetClampedToMaxSize(MaxSize);
	}

	/** Create a copy of this vector, with the maximum 2D magnitude clamped to MaxSize. Z is unchanged. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetClampedToMaxSize2D(FVector Vector, float MaxSize)
	{
		return Vector.GetClampedToMaxSize2D(MaxSize);
	}

	/**
	 * Add a vector to this and clamp the result in a cube.
	 *
	 * @param Other Vector to add.
	 * @param Radius Half size of the cube.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector AddBounded(FVector Vector, FVector Other, float Radius = 32767.f)
	{
		Vector.AddBounded(Other, Radius);
		return Vector;
	}

	/**
	 * Gets the reciprocal of this vector, avoiding division by zero.
	 * Zero components are set to BIG_NUMBER.
	 *
	 * @return Reciprocal of this vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Reciprocal(FVector Vector)
	{
		return Vector.Reciprocal();
	}

	/**
	 * Check whether X, Y and Z are nearly equal.
	 *
	 * @param Tolerance Specified Tolerance.
	 * @return true if X == Y == Z within the specified tolerance.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool IsUniform(FVector Vector, float Tolerance = 0.0001f)
	{
		return Vector.IsUniform(Tolerance);
	}

	/**
	 * Mirror a vector about a normal vector.
	 *
	 * @param MirrorNormal Normal vector to mirror about.
	 * @return Mirrored vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector MirrorByVector(FVector Vector, FVector MirrorNormal)
	{
		return Vector.MirrorByVector(MirrorNormal);
	}

	/**
	 * Mirrors a vector about a plane.
	 *
	 * @param Plane Plane to mirror about.
	 * @return Mirrored vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector MirrorByPlane(FVector Vector, const FPlane& Plane)
	{
		return Vector.MirrorByPlane(Plane);
	}

	/**
	 * Rotates around Axis (assumes Axis.Size() == 1).
	 *
	 * @param AngleDeg Angle to rotate (in degrees).
	 * @param Axis Axis to rotate around.
	 * @return Rotated Vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector RotateAngleAxis(FVector Vector, float AngleDeg, FVector Axis)
	{
		return Vector.RotateAngleAxis(AngleDeg, Axis);
	}

	/**
	 * Gets a normalized copy of the vector, checking it is safe to do so based on the length.
	 * Returns zero vector if vector length is too small to safely normalize.
	 *
	 * @param Tolerance Minimum squared vector length.
	 * @return A normalized copy if safe, (0,0,0) otherwise.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetSafeNormal(FVector Vector, float Tolerance = 0.00000001f)
	{
		return Vector.GetSafeNormal(Tolerance);
	}

	/**
	 * Gets a normalized copy of the 2D components of the vector, checking it is safe to do so. Z is set to zero. 
	 * Returns zero vector if vector length is too small to normalize.
	 *
	 * @param Tolerance Minimum squared vector length.
	 * @return Normalized copy if safe, otherwise returns zero vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetSafeNormal2D(FVector Vector, float Tolerance = 0.00000001f)
	{
		return Vector.GetSafeNormal2D(Tolerance);
	}

	/**
	 * Returns the cosine of the angle between this vector and another projected onto the XY plane (no Z).
	 *
	 * @param Other the other vector to find the 2D cosine of the angle with.
	 * @return The cosine.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float CosineAngle2D(FVector Vector, FVector Other)
	{
		return Vector.CosineAngle2D(Other);
	}

	/**
	 * Gets a copy of this vector projected onto the input vector.
	 *
	 * @param Other Vector to project onto, does not assume it is normalized.
	 * @return Projected vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector ProjectOnTo(FVector Vector, FVector Other)
	{
		return Vector.ProjectOnTo(Other);
	}

	/**
	 * Gets a copy of this vector projected onto the input vector, which is assumed to be unit length.
	 * 
	 * @param  Normal Vector to project onto (assumed to be unit length).
	 * @return Projected vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector ProjectOnToNormal(FVector Vector, FVector Normal)
	{
		return Vector.ProjectOnToNormal(Normal);
	}

	/**
	 * Return the FRotator orientation corresponding to the direction in which the vector points.
	 * Sets Yaw and Pitch to the proper numbers, and sets Roll to zero because the roll can't be determined from a vector.
	 *
	 * @return FRotator from the Vector's direction, without any roll.
	 * @see ToOrientationQuat()
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator ToRotator(FVector Vector)
	{
		return Vector.ToOrientationRotator();
	}

	/**
	 * Return the Quaternion orientation corresponding to the direction in which the vector points.
	 * Similar to the FRotator version, returns a result without roll such that it preserves the up vector.
	 *
	 * @note If you don't care about preserving the up vector and just want the most direct rotation, you can use the faster
	 * 'FQuat::FindBetweenVectors(FVector::ForwardVector, YourVector)' or 'FQuat::FindBetweenNormals(...)' if you know the vector is of unit length.
	 *
	 * @return Quaternion from the Vector's direction, without any roll.
	 * @see ToOrientationRotator(), FQuat::FindBetweenVectors()
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FQuat ToQuat(FVector Vector)
	{
		return Vector.ToOrientationQuat();
	}

	/**
	 * Find good arbitrary axis vectors to represent U and V axes of a plane,
	 * using this vector as the normal of the plane.
	 *
	 * @param Axis1 Reference to first axis.
	 * @param Axis2 Reference to second axis.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static void FindBestAxisVectors(FVector Vector, FVector& Axis1, FVector& Axis2)
	{
		return Vector.FindBestAxisVectors(Axis1, Axis2);
	}

	/** When this vector contains Euler angles (degrees), ensure that angles are between +/-180 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector UnwindEuler(FVector Vector)
	{
		Vector.UnwindEuler();
		return Vector;
	}

	/**
	 * Utility to check if there are any non-finite values (NaN or Inf) in this vector.
	 *
	 * @return true if there are any non-finite values in this vector, false otherwise.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary, DisplayName="Contains NaN"))
	static bool ContainsNaN(FVector Vector)
	{
		return Vector.ContainsNaN();
	}

	/**
	 * Check if the vector is of unit length, with specified tolerance.
	 *
	 * @param LengthSquaredTolerance Tolerance against squared length.
	 * @return true if the vector is a unit vector within the specified tolerance.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool IsUnit(FVector Vector, float LengthSquaredTolerance = 0.0001f)
	{
		return Vector.IsUnit(LengthSquaredTolerance);
	}

	/**
	 * Get a textual representation of this vector.
	 *
	 * @return A string describing the vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FString ToString(FVector Vector)
	{
		return Vector.ToString();
	}

	/**
	 * Get a locale aware textual representation of this vector.
	 *
	 * @return A string describing the vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FText ToText(FVector Vector)
	{
		return Vector.ToText();
	}

	/** Get a short textural representation of this vector, for compact readable logging. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FString ToCompactString(FVector Vector)
	{
		return Vector.ToCompactString();
	}

	/** Get a short locale aware textural representation of this vector, for compact readable logging. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FText ToCompactText(FVector Vector)
	{
		return Vector.ToCompactText();
	}

	/** 
	 * Converts a Cartesian unit vector into spherical coordinates on the unit sphere.
	 * @return Output Theta will be in the range [0, PI], and output Phi will be in the range [-PI, PI]. 
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector2D UnitCartesianToSpherical(FVector Vector)
	{
		return Vector.UnitCartesianToSpherical();
	}

	/**
	 * Convert a direction vector into a 'heading' angle.
	 *
	 * @return 'Heading' angle between +/-PI. 0 is pointing down +X.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float HeadingAngle(FVector Vector)
	{
		return Vector.HeadingAngle();
	}

	/**
	 * Compare two points and see if they're the same, using a threshold.
	 *
	 * @param P First vector.
	 * @param Q Second vector.
	 * @return Whether points are the same within a threshold. Uses fast distance approximation (linear per-component distance).
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool PointsAreSame(FVector P, FVector Q)
	{
		return FVector::PointsAreSame(P, Q);
	}

	/**
	 * Compare two points and see if they're within specified distance.
	 *
	 * @param Point1 First vector.
	 * @param Point2 Second vector.
	 * @param Dist Specified distance.
	 * @return Whether two points are within the specified distance. Uses fast distance approximation (linear per-component distance).
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool PointsAreNear(FVector Point1, FVector Point2, float Dist)
	{
		return FVector::PointsAreNear(Point1, Point2, Dist);
	}

	/**
	 * Calculate the signed distance (in the direction of the normal) between a point and a plane.
	 *
	 * @param Point The Point we are checking.
	 * @param PlaneBase The Base Point in the plane.
	 * @param PlaneNormal The Normal of the plane (assumed to be unit length).
	 * @return Signed distance between point and plane.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float PointPlaneDist(FVector Point, FVector PlaneBase, FVector PlaneNormal)
	{
		return FVector::PointPlaneDist(Point, PlaneBase, PlaneNormal);
	}

	/**
	 * Calculate the projection of a point on the given plane.
	 *
	 * @param Point The point to project onto the plane
	 * @param Plane The plane
	 * @return Projection of Point onto Plane
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector PointPlaneProject(FVector Point, const FPlane& Plane)
	{
		return FVector::PointPlaneProject(Point, Plane);
	}

	/**
	 * Calculate the projection of a vector on the plane defined by PlaneNormal.
	 * 
	 * @param  V The vector to project onto the plane.
	 * @param  PlaneNormal Normal of the plane (assumed to be unit length).
	 * @return Projection of V onto plane.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector VectorPlaneProject(FVector V, FVector PlaneNormal)
	{
		return FVector::VectorPlaneProject(V, PlaneNormal);
	}

	/**
	 * Euclidean distance between two points.
	 *
	 * @param Other The second point.
	 * @return The distance between two points.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float Dist(FVector Vector, FVector Other)
	{
		return FVector::Dist(Vector, Other);
	}

	/**
	 * Euclidean distance between two points in the XY plane (ignoring Z).
	 *
	 * @param Other The second point.
	 * @return The distance between two points in the XY plane.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float Dist2D(FVector Vector, FVector Other)
	{
		return FVector::Dist2D(Vector, Other);
	}

	/**
	 * Squared distance between two points.
	 *
	 * @param Other The second point.
	 * @return The squared distance between two points.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float DistSquared(FVector Vector, FVector Other)
	{
		return FVector::DistSquared(Vector, Other);
	}

	/**
	 * Squared distance between two points in the XY plane only.
	 *	
	 * @param Other The second point.
	 * @return The squared distance between two points in the XY plane
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float DistSquared2D(FVector Vector, FVector Other)
	{
		return FVector::DistSquared2D(Vector, Other);
	}

	/**
	 * Compute pushout of a box from a plane.
	 *
	 * @param Normal The plane normal.
	 * @param Size The size of the box.
	 * @return Pushout required.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float BoxPushOut(FVector Normal, FVector Size)
	{
		return FVector::BoxPushOut(Normal, Size);
	}

	/**
	 * See if two normal vectors are nearly parallel, meaning the angle between them is close to 0 degrees.
	 *
	 * @param  Normal1 First normalized vector.
	 * @param  Normal2 Second normalized vector.
	 * @param  ParallelCosineThreshold Normals are parallel if absolute value of dot product (cosine of angle between them) is greater than or equal to this. For example: cos(1.0 degrees).
	 * @return true if vectors are nearly parallel, false otherwise.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool Parallel(FVector Normal1, FVector Normal2, float ParallelCosineThreshold = 0.999845f)
	{
		return FVector::Parallel(Normal1, Normal2, ParallelCosineThreshold);
	}

	/**
	 * See if two normal vectors are coincident (nearly parallel and point in the same direction).
	 * 
	 * @param  Normal1 First normalized vector.
	 * @param  Normal2 Second normalized vector.
	 * @param  ParallelCosineThreshold Normals are coincident if dot product (cosine of angle between them) is greater than or equal to this. For example: cos(1.0 degrees).
	 * @return true if vectors are coincident (nearly parallel and point in the same direction), false otherwise.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool Coincident(FVector Normal1, FVector Normal2, float ParallelCosineThreshold = 0.999845f)
	{
		return FVector::Coincident(Normal1, Normal2, ParallelCosineThreshold);
	}

	/**
	 * See if two normal vectors are nearly orthogonal (perpendicular), meaning the angle between them is close to 90 degrees.
	 * 
	 * @param  Normal1 First normalized vector.
	 * @param  Normal2 Second normalized vector.
	 * @param  OrthogonalCosineThreshold Normals are orthogonal if absolute value of dot product (cosine of angle between them) is less than or equal to this. For example: cos(89.0 degrees).
	 * @return true if vectors are orthogonal (perpendicular), false otherwise.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool Orthogonal(FVector Normal1, FVector Normal2, float OrthogonalCosineThreshold = 0.017455f)
	{
		return FVector::Orthogonal(Normal1, Normal2, OrthogonalCosineThreshold);
	}

	/**
	 * See if two planes are coplanar. They are coplanar if the normals are nearly parallel and the planes include the same set of points.
	 *
	 * @param Base1 The base point in the first plane.
	 * @param Normal1 The normal of the first plane.
	 * @param Base2 The base point in the second plane.
	 * @param Normal2 The normal of the second plane.
	 * @param ParallelCosineThreshold Normals are parallel if absolute value of dot product is greater than or equal to this.
	 * @return true if the planes are coplanar, false otherwise.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool Coplanar(FVector Base1, FVector Normal1, FVector Base2, FVector Normal2, float ParallelCosineThreshold = 0.999845f)
	{
		return FVector::Coplanar(Base1, Normal1, Base2, Normal2, ParallelCosineThreshold);
	}

	/**
	 * Triple product of three vectors: X dot (Y cross Z).
	 *
	 * @param X The first vector.
	 * @param Y The second vector.
	 * @param Z The third vector.
	 * @return The triple product: X dot (Y cross Z).
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float Triple(FVector X, FVector Y, FVector Z)
	{
		return FVector::Triple(X, Y, Z);
	}

	/**
	 * Generates a list of sample points on a Bezier curve defined by 2 points.
	 *
	 * @param ControlPoints	Array of 4 FVectors (vert1, controlpoint1, controlpoint2, vert2).
	 * @param NumPoints Number of samples.
	 * @param OutPoints Receives the output samples.
	 * @return The path length.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float EvaluateBezier(const TArray<FVector>& ControlPoints, TArray<FVector>& OutPoints)
	{
		return FVector::EvaluateBezier(ControlPoints.GetData(), ControlPoints.Num(), OutPoints);
	}

	/**
	 * Converts a vector containing radian values to a vector containing degree values.
	 *
	 * @return Vector  containing degree values
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector RadiansToDegrees(FVector Vector)
	{
		return FVector::RadiansToDegrees(Vector);
	}

	/**
	 * Converts a vector containing degree values to a vector containing radian values.
	 *
	 * @return Vector containing radian values
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector DegreesToRadians(FVector Vector)
	{
		return FVector::DegreesToRadians(Vector);
	}

	/**
	 * Given a current set of cluster centers, a set of points, iterate N times to move clusters to be central. 
	 *
	 * @param Clusters Reference to array of Clusters.
	 * @param Points Set of points.
	 * @param NumIterations Number of iterations.
	 * @param NumConnectionsToBeValid Sometimes you will have long strings that come off the mass of points
	 * which happen to have been chosen as Cluster starting points.  You want to be able to disregard those.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static void GenerateClusterCenters(TArray<FVector>& Clusters, const TArray<FVector>& Points, int32 NumIterations, int32 NumConnectionsToBeValid)
	{
		return FVector::GenerateClusterCenters(Clusters, Points, NumIterations, NumConnectionsToBeValid);
	}

	/**
	 * Creates a copy of the vector
	 *
	 * @return A copy of the original vector
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Clone(FVector Vector)
	{
		return Vector;
	}

	/** Linearly interpolates between self and other based on Alpha (100% of A when Alpha=0 and 100% of B when Alpha=1) */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Lerp(FVector Vector, FVector Other, float Alpha)
	{
		return UKismetMathLibrary::VLerp(Vector, Other, Alpha);
	}

	/** Easeing between self and other using a specified easing function */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector Ease(
		FVector Vector,
		FVector Other,
		float Alpha,
		TEnumAsByte<EEasingFunc::Type> EasingFunc,
		float BlendExp = 2,
		int32 Steps = 2)
	{
		return UKismetMathLibrary::VEase(
			Vector,
			Other,
			Alpha,
			EasingFunc,
			BlendExp,
			Steps);
	}

	/**
	 * Tries to reach Target based on distance from Current position, giving a nice smooth feeling when tracking a position.
	 *
	 * @param		Current			Actual position
	 * @param		Target			Target position
	 * @param		DeltaTime		Time since last tick
	 * @param		InterpSpeed		Interpolation speed
	 * @return		New interpolated position
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector InterpTo(
		FVector Current,
		FVector Target,
		float DeltaTime,
		float InterpSpeed)
	{
		return UKismetMathLibrary::VInterpTo(Current, Target, DeltaTime, InterpSpeed);
	}

	/**
	 * Tries to reach Target at a constant rate.
	 *
	 * @param		Current			Actual position
	 * @param		Target			Target position
	 * @param		DeltaTime		Time since last tick
	 * @param		InterpSpeed		Interpolation speed
	 * @return		New interpolated position
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector InterpToConstant(
		FVector Current,
		FVector Target,
		float DeltaTime,
		float InterpSpeed)
	{
		return UKismetMathLibrary::VInterpTo_Constant(Current, Target, DeltaTime, InterpSpeed);
	}

	/** Copies the vector, but with a new X-value. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector WithX(FVector Vector, float Value)
	{
		Vector.X = Value;
		return Vector;
	}

	/** Copies the vector, but with a new Y-value. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector WithY(FVector Vector, float Value)
	{
		Vector.Y = Value;
		return Vector;
	}

	/** Copies the vector, but with a new Z-value. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector WithZ(FVector Vector, float Value)
	{
		Vector.Z = Value;
		return Vector;
	}
};
