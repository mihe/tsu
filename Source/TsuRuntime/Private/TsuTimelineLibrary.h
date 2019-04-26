#pragma once

// #hack(#mihe): Here be dragons. We'll do this until we can get a PR through to extend the API.
// #note(#mihe): Don't include anything above this
#pragma push_macro("private")
#define private public
#include "Components/TimelineComponent.h"
#pragma pop_macro("private");

#include "CoreMinimal.h"

#include "TsuRuntimeLog.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "TsuTimelineLibrary.generated.h"

UCLASS(Meta=(BlueprintInternalUseOnly, TsuExtension))
class UTsuTimelineLibrary final
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void AddFloatCurve(
		UTimelineComponent* Timeline,
		FName TrackName,
		UCurveFloat* Curve)
	{
		Timeline->AddInterpFloat(Curve, {}, NAME_None, TrackName);
	}

	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void AddVectorCurve(
		UTimelineComponent* Timeline,
		FName TrackName,
		UCurveVector* Curve)
	{
		Timeline->AddInterpVector(Curve, {}, NAME_None, TrackName);
	}

	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void AddLinearColorCurve(
		UTimelineComponent* Timeline,
		FName TrackName,
		UCurveLinearColor* Curve)
	{
		Timeline->AddInterpLinearColor(Curve, {}, NAME_None, TrackName);
	}

	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void RemoveFloatCurve(UTimelineComponent* Timeline, FName TrackName)
	{
		Timeline->TheTimeline.InterpFloats.RemoveAll(
			[&](const FTimelineFloatTrack& Track)
			{
				return Track.TrackName == TrackName;
			});
	}

	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void RemoveVectorCurve(UTimelineComponent* Timeline, FName TrackName)
	{
		Timeline->TheTimeline.InterpVectors.RemoveAll(
			[&](const FTimelineVectorTrack& Track)
			{
				return Track.TrackName == TrackName;
			});
	}

	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void RemoveLinearColorCurve(UTimelineComponent* Timeline, FName TrackName)
	{
		Timeline->TheTimeline.InterpLinearColors.RemoveAll(
			[&](const FTimelineLinearColorTrack& Track)
			{
				return Track.TrackName == TrackName;
			});
	}

	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetOnFloatUpdate(UTimelineComponent* Timeline, FName TrackName, FOnTimelineFloat Callback)
	{
		FTimelineFloatTrack* FoundTrack = Timeline->TheTimeline.InterpFloats.FindByPredicate(
			[&](const FTimelineFloatTrack& Track)
			{
				return Track.TrackName == TrackName;
			});

		if (FoundTrack)
		{
			FoundTrack->InterpFunc = Callback;
		}
		else
		{
			UE_LOG(LogTsuRuntime, Warning, TEXT("Failed to find float track by name: %s"), *TrackName.ToString());
		}
	}

	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetOnVectorUpdate(UTimelineComponent* Timeline, FName TrackName, FOnTimelineVector Callback)
	{
		FTimelineVectorTrack* FoundTrack = Timeline->TheTimeline.InterpVectors.FindByPredicate(
			[&](const FTimelineVectorTrack& Track)
			{
				return Track.TrackName == TrackName;
			});

		if (FoundTrack)
		{
			FoundTrack->InterpFunc = Callback;
		}
		else
		{
			UE_LOG(LogTsuRuntime, Warning, TEXT("Failed to find vector track by name: %s"), *TrackName.ToString());
		}
	}

	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetOnLinearColorUpdate(UTimelineComponent* Timeline, FName TrackName, FOnTimelineLinearColor Callback)
	{
		FTimelineLinearColorTrack* FoundTrack = Timeline->TheTimeline.InterpLinearColors.FindByPredicate(
			[&](const FTimelineLinearColorTrack& Track)
			{
				return Track.TrackName == TrackName;
			});

		if (FoundTrack)
		{
			FoundTrack->InterpFunc = Callback;
		}
		else
		{
			UE_LOG(LogTsuRuntime, Warning, TEXT("Failed to find linear color track by name: %s"), *TrackName.ToString());
		}
	}

	/** */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetOnFinished(UTimelineComponent* Timeline, FOnTimelineEvent Callback)
	{
		Timeline->SetTimelineFinishedFunc(Callback);
	}
};
