// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphicsSettingsWidget.h"

#include "GameFramework/GameUserSettings.h"
#include "DustFall/UI/Widgets/Settings/Items/SettingItem_Selector.h"

void UGraphicsSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const UGameUserSettings* UserSettings = GEngine->GetGameUserSettings())
	{
		if (Selector_TextureQuality)
		{
			Selector_TextureQuality->SetSelectedIndex(UserSettings->GetTextureQuality());
			Selector_TextureQuality->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangeTextureQuality);	
		}

		if (Selector_ShadowQuality)
		{
			Selector_ShadowQuality->SetSelectedIndex(UserSettings->GetShadowQuality());
			Selector_ShadowQuality->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangeShadowQuality);	
		}

		if (Selector_PostProcessingQuality)
		{
			Selector_PostProcessingQuality->SetSelectedIndex(UserSettings->GetPostProcessingQuality());
			Selector_PostProcessingQuality->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangePostProcessingQuality);	
		}

		if (Selector_GlobalIllumination)
		{
			Selector_GlobalIllumination->SetSelectedIndex(UserSettings->GetGlobalIlluminationQuality());
			Selector_GlobalIllumination->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangeGlobalIlluminationQuality);	
		}

		if (Selector_Reflections)
		{
			Selector_Reflections->SetSelectedIndex(UserSettings->GetReflectionQuality());
			Selector_Reflections->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangeReflectionsQuality);	
		}

		if (Selector_Foliage)
		{
			Selector_Foliage->SetSelectedIndex(UserSettings->GetFoliageQuality());
			Selector_Foliage->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangeFoliageQuality);	
		}

		if (Selector_Shading)
		{
			Selector_Shading->SetSelectedIndex(UserSettings->GetShadingQuality());
			Selector_Shading->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangeShadingQuality);	
		}

		if (Selector_EffectsQuality)
		{
			Selector_EffectsQuality->SetSelectedIndex(UserSettings->GetVisualEffectQuality());
			Selector_EffectsQuality->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangeEffectsQuality);	
		}

		if (Selector_ViewDistanceQuality)
		{
			Selector_ViewDistanceQuality->SetSelectedIndex(UserSettings->GetViewDistanceQuality());
			Selector_ViewDistanceQuality->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangeViewDistanceQuality);	
		}

		if (Selector_AntiAliasing)
		{
			Selector_AntiAliasing->SetSelectedIndex(UserSettings->GetViewDistanceQuality());
			Selector_AntiAliasing->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnChangeAntiAliasing);	
		}
	}
}

void UGraphicsSettingsWidget::OnChangeTextureQuality(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetTextureQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangeShadowQuality(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetShadowQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangePostProcessingQuality(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetPostProcessingQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangeGlobalIlluminationQuality(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetGlobalIlluminationQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangeReflectionsQuality(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetReflectionQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangeFoliageQuality(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetFoliageQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangeShadingQuality(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetShadingQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangeEffectsQuality(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetVisualEffectQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangeViewDistanceQuality(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetViewDistanceQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangeAntiAliasing(const int32& Index)
{
	ApplySetting([Index](UGameUserSettings* Settings) {
		Settings->SetAntiAliasingQuality(Index);
	});
}

void UGraphicsSettingsWidget::OnChangeAnisotropicFiltering(const int32& Index)
{
	//TODO: OnChangeAnisotropicFiltering
}

void UGraphicsSettingsWidget::OnChangeMotionBlurQuality(const int32& Index)
{
	//TODO: OnChangeMotionBlurQuality
}

void UGraphicsSettingsWidget::OnChangeAmbientOcclusion(const int32& Index)
{
	//TODO: OnChangeAmbientOcclusion
}

void UGraphicsSettingsWidget::OnChangeDepthOfField(const int32& Index)
{
	//TODO: OnChangeDepthOfField
}

void UGraphicsSettingsWidget::OnChangeBloom(const int32& Index)
{
	//TODO: OnChangeBloom
}

void UGraphicsSettingsWidget::OnChangeScreenSpaceReflections(const int32& Index)
{
	//TODO: OnChangeScreenSpaceReflections
}

void UGraphicsSettingsWidget::OnChangeTessellation(const int32& Index)
{
	//TODO: OnChangeTessellation
}

void UGraphicsSettingsWidget::OnChangeRayTracing(const bool& Value)
{
	
}

void UGraphicsSettingsWidget::ApplySetting(TFunctionRef<void(UGameUserSettings*)> ApplyFunc)
{
	if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
	{
		ApplyFunc(Settings);
		Settings->ApplySettings(false);
	}
}