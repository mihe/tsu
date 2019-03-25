#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "TsuTransformLibrary.generated.h"

UCLASS(ClassGroup=TSU)
class UTsuTransformLibrary final
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Get a textual representation of the transform.
	 *
	 * @return Text describing the vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FString ToHumanReadableString(const FTransform& Transform)
	{
		return Transform.ToHumanReadableString();
	}

	/**
	 * Get a textual representation of the transform.
	 *
	 * @return Text describing the vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FString ToString(const FTransform& Transform)
	{
		return Transform.ToString();
	}

	/** Convert this Transform to a transformation matrix with scaling. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FMatrix ToMatrix(const FTransform& Transform)
	{
		return Transform.ToMatrixWithScale();
	}

	/** Convert this Transform to inverse. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform Inverse(const FTransform& Transform)
	{
		return Transform.Inverse();
	}

	/** Calculate the weighted blend of this Transform and the supplied Transform. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform BlendedWith(FTransform Transform, const FTransform& Other, float Alpha)
	{
		Transform.BlendWith(Other, Alpha);
		return Transform;
	}

	/**
	 * Return a transform that is the result of this multiplied by another transform.
	 * Order matters when composing transforms : C = A * B will yield a transform C that logically first applies A then B to any subsequent transformation.
	 *
	 * @param  Other other transform by which to multiply.
	 * @return new transform: this * Other
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform Multiply(const FTransform& Transform, const FTransform& Other)
	{
		return Transform * Other;
	}

	/** Scale the location part of the Transform by the supplied vector. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform ScaleLocation(FTransform Transform, FVector Scale)
	{
		Transform.ScaleTranslation(Scale);
		return Transform;
	}

	/** Scale the location part of the Transform by the supplied float. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform ScaleLocationFloat(FTransform Transform, float Scale)
	{
		Transform.ScaleTranslation(Scale);
		return Transform;
	}

	/** Sets scale to (1,1,1) and normalizes rotation */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform RemoveScaling(FTransform Transform)
	{
		Transform.RemoveScaling(0.00000001f);
		return Transform;
	}

	/** Gets the maximum magnitude of any row of the matrix. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float GetMaximumAxisScale(const FTransform& Transform)
	{
		return Transform.GetMaximumAxisScale();
	}

	/** Gets the minimum magnitude of any row of the matrix. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float GetMinimumAxisScale(const FTransform& Transform)
	{
		return Transform.GetMinimumAxisScale();
	}

	/**
	 * Get delta transform in a format that can be concatenated.
	 * Inverse() itself can't concatenate with VQS format(since VQS always transform from S->Q->T, where inverse happens from T(-1)->Q(-1)->S(-1)).
	 * This provides ways to fix this.
	 * @return this*Other(-1)
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform GetRelativeTransform(const FTransform& Transform, const FTransform& Other)
	{
		return Transform.GetRelativeTransform(Other);
	}

	/**
	 * Get delta transform in a format that can be concatenated.
	 * Inverse() itself can't concatenate with VQS format(since VQS always transform from S->Q->T, where inverse happens from T(-1)->Q(-1)->S(-1)).
	 * This provides ways to fix this.
	 * @return this(-1)*Other
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform GetRelativeTransformReverse(const FTransform& Transform, const FTransform& Other)
	{
		return Transform.GetRelativeTransformReverse(Other);
	}

	/** 
	 * Transform a position by the supplied transform.
	 * For example, if T was an object's transform, this would transform a position from local space to world space.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector TransformLocation(const FTransform& Transform, FVector V)
	{
		return UKismetMathLibrary::TransformLocation(Transform, V);
	}

	/** 
	 * Transform a position by the supplied transform, ignoring the scale.
	 * For example, if T was an object's transform, this would transform a position from local space to world space.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector TransformLocationNoScale(const FTransform& Transform, FVector Location)
	{
		return Transform.TransformPositionNoScale(Location);
	}

	/** 
	 * Transform a position by the inverse of the supplied transform.
	 * For example, if T was an object's transform, this would transform a position from world space to local space.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector InverseTransformLocation(const FTransform& Transform, FVector Location)
	{
		return UKismetMathLibrary::InverseTransformLocation(Transform, Location);
	}

	/** 
	 * Transform a position by the inverse of the supplied transform, ignoring the scale.
	 * For example, if T was an object's transform, this would transform a position from world space to local space.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector InverseTransformLocationNoScale(const FTransform& Transform, FVector Location)
	{
		return Transform.InverseTransformPositionNoScale(Location);
	}

	/** 
	 *	Transform a direction vector by the supplied transform - will not change its length. 
	 *	For example, if T was an object's transform, this would transform a direction from local space to world space.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector TransformDirection(const FTransform& Transform, FVector Direction)
	{
		return UKismetMathLibrary::TransformDirection(Transform, Direction);
	}

	/** Inverts the transform and then transforms direction - correctly handles scaling in this transform. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector TransformDirectionNoScale(const FTransform& Transform, FVector Direction)
	{
		return Transform.TransformVectorNoScale(Direction);
	}

	/** 
	 * Transform a direction vector by the inverse of this transform - will not take the location part into account.
	 * If you want to transform a surface normal (or plane) and correctly account for non-uniform scaling you should use TransformByUsingAdjointT with adjoint of matrix inverse.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector InverseTransformDirection(const FTransform& Transform, FVector Direction)
	{
		return Transform.InverseTransformVector(Direction);
	}

	/** 
	 * Transform a direction vector by the inverse of this transform - will not take the location or scaling part into account.
	 * If you want to transform a surface normal (or plane) and correctly account for non-uniform scaling you should use TransformByUsingAdjointT with adjoint of matrix inverse.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector InverseTransformDirectionNoScale(const FTransform& Transform, FVector Direction)
	{
		return Transform.InverseTransformVectorNoScale(Direction);
	}

	/** 
	 * Transform a rotator by the supplied transform. 
	 * For example, if T was an object's transform, this would transform a rotation from local space to world space.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator TransformRotation(const FTransform& Transform, FRotator Rotation)
	{
		return UKismetMathLibrary::TransformRotation(Transform, Rotation);
	}

	/** Inverts the transform and then transforms Rotation */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator InverseTransformRotation(const FTransform& Transform, FRotator Rotation)
	{
		return UKismetMathLibrary::InverseTransformRotation(Transform, Rotation);
	}

	/** */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetScaledAxis(const FTransform& Transform, EAxis::Type Axis)
	{
		return Transform.GetScaledAxis(Axis);
	}

	/** */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetUnitAxis(const FTransform& Transform, EAxis::Type Axis)
	{
		return Transform.GetUnitAxis(Axis);
	}

	/** */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform Mirror(FTransform Transform, EAxis::Type MirrorAxis, EAxis::Type FlipAxis)
	{
		Transform.Mirror(MirrorAxis, FlipAxis);
		return Transform;
	}

	/** */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float GetDeterminant(const FTransform& Transform)
	{
		return Transform.GetDeterminant();
	}

	/**
	 * Checks the components for NaN's
	 * @return Returns true if any component (rotation, location, or scale) is a NaN
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary, DisplayName="Contains NaN"))
	static bool ContainsNaN(const FTransform& Transform)
	{
		return Transform.ContainsNaN();
	}

	/** */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool IsValid(const FTransform& Transform)
	{
		return Transform.IsValid();
	}

	// Test if this Transform's rotation equals another's rotation, within a tolerance. Preferred over "GetRotation().Equals(Other.GetRotation())" because it is faster on some platforms.
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool RotationEquals(const FTransform& Transform, const FTransform& Other, float Tolerance = 0.0001f)
	{
		return Transform.RotationEquals(Other, Tolerance);
	}

	// Test if this Transform's location equals another's location, within a tolerance. Preferred over "Transform.Location.Equals(Other.Location)" because it avoids VectorRegister->FVector conversion.
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool LocationEquals(const FTransform& Transform, const FTransform& Other, float Tolerance = 0.0001f)
	{
		return Transform.TranslationEquals(Other, Tolerance);
	}

	// Test if this Transform's scale equals another's scale, within a tolerance. Preferred over "Transform.Scale.Equals(Other.Scale)" because it avoids VectorRegister->FVector conversion.
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool ScaleEquals(const FTransform& Transform, const FTransform& Other, float Tolerance = 0.0001f)
	{
		return Transform.Scale3DEquals(Other, Tolerance);
	}

	// Test if all components of the transforms are equal, within a tolerance.
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool Equals(const FTransform& Transform, const FTransform& Other, float Tolerance = 0.0001f)
	{
		return Transform.Equals(Other, Tolerance);
	}

	// Test if rotation and location components of the transforms are equal, within a tolerance.
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool EqualsNoScale(const FTransform& Transform, const FTransform& Other, float Tolerance = 0.0001f)
	{
		return Transform.EqualsNoScale(Other, Tolerance);
	}

	/**
	 * Scales the Scale component by a new factor
	 * @param ScaleMultiplier The value to multiply Scale with
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform MultiplyScale(const FTransform& Transform, FVector ScaleMultiplier)
	{
		return Transform.GetScaled(ScaleMultiplier);
	}

	/**
	 * Scales the Scale component by a new factor
	 * @param ScaleMultiplier The value to multiply Scale with
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform MultiplyScaleFloat(const FTransform& Transform, float ScaleMultiplier)
	{
		return Transform.GetScaled(ScaleMultiplier);
	}

	/**
	 * Concatenates another rotation to this transformation 
	 * @param DeltaRotation The rotation to concatenate
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform ConcatenateRotation(FTransform Transform, FRotator DeltaRotation)
	{
		Transform.ConcatenateRotation(FQuat(DeltaRotation));
		return Transform;
	}

	/** */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform AddToLocation(FTransform Transform, FVector LocationDelta)
	{
		Transform.AddToTranslation(LocationDelta);
		return Transform;
	}

	/**
	 * Accumulates another transform with this one
	 *
	 * Rotation is accumulated multiplicatively (Rotation = Other.Rotation * Rotation)
	 * Location is accumulated additively (Location += Other.Location)
	 * Scale is accumulated multiplicatively (Scale *= Other.Scale)
	 *
	 * @param Other The other transform to accumulate into this one
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform Accumulate(FTransform Transform, const FTransform& Other)
	{
		Transform.Accumulate(Other);
		return Transform;
	}

	/**
	 * Normalize the rotation component of this transformation
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform NormalizeRotation(FTransform Transform)
	{
		Transform.NormalizeRotation();
		return Transform;
	}

	/**
	 * Checks whether the rotation component is normalized or not
	 * @return true if the rotation component is normalized, and false otherwise.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool IsRotationNormalized(const FTransform& Transform)
	{
		return Transform.IsRotationNormalized();
	}

	/** Linearly interpolates between this and Other based on Alpha (100% of A when Alpha=0 and 100% of B when Alpha=1). */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform Lerp(
		const FTransform& Transform,
		const FTransform& Other,
		float Alpha,
		TEnumAsByte<ELerpInterpolationMode::Type> Mode = ELerpInterpolationMode::QuatInterp)
	{
		return UKismetMathLibrary::TLerp(Transform, Other, Alpha, Mode);
	}

	/** Ease between this and Other using a specified easing function. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform Ease(
		const FTransform& Transform,
		const FTransform& Other,
		float Alpha,
		TEnumAsByte<EEasingFunc::Type> EasingFunc,
		float BlendExp = 2,
		int32 Steps = 2)
	{
		return UKismetMathLibrary::TEase(Transform, Other, Alpha, EasingFunc, BlendExp, Steps);
	}

	/** Tries to reach a target transform. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform InterpTo(
		const FTransform& Current,
		const FTransform& Target,
		float DeltaTime,
		float InterpSpeed)
	{
		return UKismetMathLibrary::TInterpTo(Current, Target, DeltaTime, InterpSpeed);
	}

	/** Copies the transform, but with a new location value. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform WithLocation(FTransform Transform, FVector Location)
	{
		Transform.SetTranslation(Location);
		return Transform;
	}

	/** Copies the transform, but with a new scale value. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform WithScale(FTransform Transform, FVector Scale)
	{
		Transform.SetScale3D(Scale);
		return Transform;
	}

	/** Copies the transform, but with a new rotator value. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FTransform WithRotation(FTransform Transform, FRotator Rotation)
	{
		Transform.SetRotation(FQuat(Rotation));
		return Transform;
	}
};
