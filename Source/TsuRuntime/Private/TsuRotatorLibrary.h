#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "TsuRotatorLibrary.generated.h"

UCLASS(ClassGroup=TSU)
class UTsuRotatorLibrary final
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Get the result of adding a rotator to this.
	 *
	 * @param Other The other rotator.
	 * @return The result of adding a rotator to this.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator Add(FRotator Rotator, FRotator Other)
	{
		return Rotator + Other;
	}

	/**
	 * Adds to each component of the rotator.
	 *
	 * @param DeltaPitch Change in pitch. (+/-)
	 * @param DeltaYaw Change in yaw. (+/-)
	 * @param DeltaRoll Change in roll. (+/-)
	 * @return Copy of rotator after addition.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator AddFloats(FRotator Rotator, float DeltaPitch, float DeltaYaw, float DeltaRoll)
	{
		return Rotator.Add(DeltaPitch, DeltaYaw, DeltaRoll);
	}

	/**
	 * Get the result of subtracting a rotator from this.
	 *
	 * @param Other The other rotator.
	 * @return The result of subtracting a rotator from this.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator Subtract(FRotator Rotator, FRotator Other)
	{
		return Rotator - Other;
	}

	/**
	 * Get the result of scaling this rotator.
	 *
	 * @param Scale The scaling factor.
	 * @return The result of scaling.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator Scale(FRotator Rotator, float Scale)
	{
		return Rotator * Scale;
	}

	/**
	 * Checks whether rotator is nearly zero within specified tolerance, when treated as an orientation.
	 * This means that FRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator.
	 *
	 * @param Tolerance Error Tolerance.
	 * @return true if rotator is nearly zero, within specified tolerance, otherwise false.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool IsNearlyZero(FRotator Rotator, float Tolerance = 0.0001f)
	{
		return Rotator.IsNearlyZero(Tolerance);
	}

	/**
	 * Checks whether this has exactly zero rotation, when treated as an orientation.
	 * This means that FRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator.
	 *
	 * @return true if this has exactly zero rotation, otherwise false.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool IsZero(FRotator Rotator)
	{
		return Rotator.IsZero();
	}

	/**
	 * Checks whether two rotators are equal within specified tolerance, when treated as an orientation.
	 * This means that FRotator(0, 0, 360).Equals(FRotator(0,0,0)) is true, because they represent the same final orientation.
	 *
	 * @param Other The other rotator.
	 * @param Tolerance Error Tolerance.
	 * @return true if two rotators are equal, within specified tolerance, otherwise false.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static bool Equals(FRotator Rotator, FRotator Other, float Tolerance = 0.0001f)
	{
		return Rotator.Equals(Other, Tolerance);
	}

	/**
	 * Returns the inverse of the rotator.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator GetInverse(FRotator Rotator)
	{
		return Rotator.GetInverse();
	}

	/**
	 * Get the rotation, snapped to specified degree segments.
	 *
	 * @param RotGrid A Rotator specifying how to snap each component.
	 * @return Snapped version of rotation.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator GridSnap(FRotator Rotator, FRotator RotGrid)
	{
		return Rotator.GridSnap(RotGrid);
	}

	/**
	 * Convert a rotation into a unit vector facing in its direction.
	 *
	 * @return Rotation as a unit direction vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector ToVector(FRotator Rotator)
	{
		return Rotator.Vector();
	}

	/**
	 * Get Rotation as a quaternion.
	 *
	 * @return Rotation as a quaternion.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FQuat ToQuaternion(FRotator Rotator)
	{
		return Rotator.Quaternion();
	}

	/**
	 * Convert a Rotator into floating-point Euler angles (in degrees). Rotator now stored in degrees.
	 *
	 * @return Rotation as a Euler angle vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector ToEuler(FRotator Rotator)
	{
		return Rotator.Euler();
	}

	/**
	 * Rotate a vector rotated by this rotator.
	 *
	 * @param V The vector to rotate.
	 * @return The rotated vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector RotateVector(FRotator Rotator, FVector V)
	{
		return Rotator.RotateVector(V);
	}

	/**
	 * Returns the vector rotated by the inverse of this rotator.
	 *
	 * @param V The vector to rotate.
	 * @return The rotated vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector UnrotateVector(FRotator Rotator, FVector V)
	{
		return Rotator.UnrotateVector(V);
	}

	/**
	 * Gets the rotation values so they fall within the range [0,360]
	 *
	 * @return Clamped version of rotator.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator Clamp(FRotator Rotator)
	{
		return Rotator.Clamp();
	}

	/** 
	 * Create a copy of this rotator and normalize, removes all winding and creates the "shortest route" rotation. 
	 *
	 * @return Normalized copy of this rotator
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator GetNormalized(FRotator Rotator)
	{
		return Rotator.GetNormalized();
	}

	/** 
	 * Create a copy of this rotator and denormalize, clamping each axis to 0 - 360. 
	 *
	 * @return Denormalized copy of this rotator
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator GetDenormalized(FRotator Rotator)
	{
		return Rotator.GetDenormalized();
	}

	/** 
	 * Decompose this Rotator into a Winding part (multiples of 360) and a Remainder part. 
	 * Remainder will always be in [-180, 180] range.
	 *
	 * @param Winding[Out] the Winding part of this Rotator
	 * @param Remainder[Out] the Remainder
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static void GetWindingAndRemainder(FRotator Rotator, FRotator& Winding, FRotator& Remainder)
	{
		return Rotator.GetWindingAndRemainder(Winding, Remainder);
	}

	/**
	 * Get a textual representation of the vector.
	 *
	 * @return Text describing the vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FString ToString(FRotator Rotator)
	{
		return Rotator.ToString();
	}

	/** Get a short textural representation of this vector, for compact readable logging. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FString ToCompactString(FRotator Rotator)
	{
		return Rotator.ToCompactString();
	}

	/**
	 * Utility to check if there are any non-finite values (NaN or Inf) in this Rotator.
	 *
	 * @return true if there are any non-finite values in this Rotator, otherwise false.
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary, DisplayName="Contains NaN"))
	static bool ContainsNaN(FRotator Rotator)
	{
		return Rotator.ContainsNaN();
	}

	/** Generates a random rotation, with optional random roll. */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator Random(bool bRoll = false)
	{
		return UKismetMathLibrary::RandomRotator(bRoll);
	}

	/**
	 * Clamps an angle to the range of [0, 360).
	 *
	 * @param Angle The angle to clamp.
	 * @return The clamped angle.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float ClampAxis(float Angle)
	{
		return FRotator::ClampAxis(Angle);
	}

	/**
	 * Clamps an angle to the range of (-180, 180].
	 *
	 * @param Angle The Angle to clamp.
	 * @return The clamped angle.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float NormalizeAxis(float Angle)
	{
		return FRotator::NormalizeAxis(Angle);
	}

	/**
	 * Compresses a floating point angle into a byte.
	 *
	 * @param Angle The angle to compress.
	 * @return The angle as a byte.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static uint8 CompressAxisToByte(float Angle)
	{
		return FRotator::CompressAxisToByte(Angle);
	}

	/**
	 * Decompress a word into a floating point angle.
	 *
	 * @param Angle The word angle.
	 * @return The decompressed angle.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float DecompressAxisFromByte(uint16 Angle)
	{
		return FRotator::DecompressAxisFromByte(Angle);
	}

	/**
	 * Compress a floating point angle into a word.
	 *
	 * @param Angle The angle to compress.
	 * @return The decompressed angle.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static uint16 CompressAxisToShort(float Angle)
	{
		return FRotator::CompressAxisToShort(Angle);
	}

	/**
	 * Decompress a short into a floating point angle.
	 *
	 * @param Angle The word angle.
	 * @return The decompressed angle.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static float DecompressAxisFromShort(uint16 Angle)
	{
		return FRotator::DecompressAxisFromShort(Angle);
	}

	/**
	 * Convert a vector of floating-point Euler angles (in degrees) into a Rotator. Rotator now stored in degrees
	 *
	 * @param Euler Euler angle vector.
	 * @return A rotator from a Euler angle.
	 */
	//UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator MakeFromEuler(FVector Euler)
	{
		return FRotator::MakeFromEuler(Euler);
	}

	/** Get the reference frame direction vectors (axes) described by this rotation */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static void GetAxes(FRotator Rotator, FVector& X, FVector& Y, FVector& Z)
	{
		UKismetMathLibrary::GetAxes(Rotator, X, Y, Z);
	}

	/** Rotate the world forward vector by the given rotation */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetForwardVector(FRotator Rotator)
	{
		return UKismetMathLibrary::GetForwardVector(Rotator);
	}

	/** Rotate the world right vector by the given rotation */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetRightVector(FRotator Rotator)
	{
		return UKismetMathLibrary::GetRightVector(Rotator);
	}

	/** Rotate the world up vector by the given rotation */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FVector GetUpVector(FRotator Rotator)
	{
		return UKismetMathLibrary::GetUpVector(Rotator);
	}

	/** Linearly interpolates between A and B based on Alpha (100% of A when Alpha=0 and 100% of B when Alpha=1) */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator Lerp(FRotator Rotator, FRotator Other, float Alpha, bool bShortestPath)
	{
		return UKismetMathLibrary::RLerp(Rotator, Other, Alpha, bShortestPath);
	}

	/** Easeing  between A and B using a specified easing function */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator Ease(
		FRotator Rotator,
		FRotator Other,
		float Alpha,
		bool bShortestPath,
		TEnumAsByte<EEasingFunc::Type> EasingFunc,
		float BlendExp = 2,
		int32 Steps = 2)
	{
		return UKismetMathLibrary::REase(
			Rotator,
			Other,
			Alpha,
			bShortestPath,
			EasingFunc,
			BlendExp,
			Steps);
	}

	/**
	 * Tries to reach Target rotation based on Current rotation, giving a nice smooth feeling when rotating to Target rotation.
	 *
	 * @param		Current			Actual rotation
	 * @param		Target			Target rotation
	 * @param		DeltaTime		Time since last tick
	 * @param		InterpSpeed		Interpolation speed
	 * @return		New interpolated position
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator InterpTo(FRotator Current, FRotator Target, float DeltaTime, float InterpSpeed)
	{
		return UKismetMathLibrary::RInterpTo(Current, Target, DeltaTime, InterpSpeed);
	}

	/**
	 * Tries to reach Target rotation at a constant rate.
	 *
	 * @param		Current			Actual rotation
	 * @param		Target			Target rotation
	 * @param		DeltaTime		Time since last tick
	 * @param		InterpSpeed		Interpolation speed
	 * @return		New interpolated position
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator InterpToConstant(FRotator Current, FRotator Target, float DeltaTime, float InterpSpeed)
	{
		return UKismetMathLibrary::RInterpTo_Constant(Current, Target, DeltaTime, InterpSpeed);
	}

	/** Combine 2 rotations to give you the resulting rotation of first applying self, then other. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator Compose(FRotator Rotator, FRotator Other)
	{
		return UKismetMathLibrary::ComposeRotators(Rotator, Other);
	}

	/** Copies the rotator, but with a new pitch value. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator WithPitch(FRotator Rotator, float Degrees)
	{
		Rotator.Pitch = Degrees;
		return Rotator;
	}

	/** Copies the rotator, but with a new yaw value. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator WithYaw(FRotator Rotator, float Degrees)
	{
		Rotator.Yaw = Degrees;
		return Rotator;
	}

	/** Copies the rotator, but with a new roll value. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static FRotator WithRoll(FRotator Rotator, float Degrees)
	{
		Rotator.Roll = Degrees;
		return Rotator;
	}
};
