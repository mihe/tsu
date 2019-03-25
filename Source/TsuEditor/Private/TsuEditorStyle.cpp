#include "TsuEditorStyle.h"

#include "TsuPaths.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define TSU_IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(TEXT(RelativePath), TEXT(".png")), __VA_ARGS__)

FSlateStyleSet* FTsuEditorStyle::StyleSet = nullptr;

void FTsuEditorStyle::Initialize()
{
	if (!ensure(!StyleSet))
		return;

	StyleSet = new FSlateStyleSet(TEXT("TsuEditorStyle"));
	StyleSet->SetContentRoot(FTsuPaths::ContentDir());

	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);

	StyleSet->Set("TsuEditor.OpenInTextEditor", new TSU_IMAGE_BRUSH("Icons/TsuBlueprint_40x", Icon40x40));
	StyleSet->Set("TsuEditor.OpenInTextEditor.Small", new TSU_IMAGE_BRUSH("Icons/TsuBlueprint_20x", Icon20x20));
	StyleSet->Set("ClassIcon.TsuBlueprint", new TSU_IMAGE_BRUSH("Icons/TsuBlueprint_16x", Icon16x16));
	StyleSet->Set("ClassIcon.TsuBlueprint.Large", new TSU_IMAGE_BRUSH("Icons/TsuBlueprint_40x", Icon40x40));
	StyleSet->Set("ClassThumbnail.TsuBlueprint", new TSU_IMAGE_BRUSH("Icons/TsuBlueprint_64x", Icon64x64));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
}

void FTsuEditorStyle::Shutdown()
{
	if (ensure(StyleSet))
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
		delete StyleSet;
		StyleSet = nullptr;
	}
}

ISlateStyle& FTsuEditorStyle::Get()
{
	return *StyleSet;
}
