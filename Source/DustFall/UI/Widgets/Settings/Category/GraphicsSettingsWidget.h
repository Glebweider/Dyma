// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphicsSettingsWidget.generated.h"

class USettingItem_CheckBox;
class USettingItem_Slider;
class USettingItem_Selector;
/**
 * 
 */
UCLASS()
class DUSTFALL_API UGraphicsSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_TextureQuality;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_ShadowQuality;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_GlobalIllumination;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_Reflections;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_Foliage;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_Shading;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_PostProcessingQuality;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_EffectsQuality;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_ViewDistanceQuality;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_AntiAliasing;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_AnisotropicFiltering;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_MotionBlurQuality;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_AmbientOcclusion;
	
	UPROPERTY(meta = (BindWidget))
	USettingItem_Selector* Selector_ScreenSpaceReflections;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Slider* Slider_DepthOfField;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Slider* Slider_Bloom;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Slider* Slider_Tessellation;

	UPROPERTY(meta = (BindWidget))
	USettingItem_CheckBox* CheckBox_RayTracing;

	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnChangeTextureQuality(const int32& Index);

	UFUNCTION()
	void OnChangeShadowQuality(const int32& Index);

	UFUNCTION()
	void OnChangePostProcessingQuality(const int32& Index);

	UFUNCTION()
	void OnChangeGlobalIlluminationQuality(const int32& Index);

	UFUNCTION()
	void OnChangeReflectionsQuality(const int32& Index);

	UFUNCTION()
	void OnChangeFoliageQuality(const int32& Index);

	UFUNCTION()
	void OnChangeShadingQuality(const int32& Index);

	UFUNCTION()
	void OnChangeEffectsQuality(const int32& Index);

	UFUNCTION()
	void OnChangeViewDistanceQuality(const int32& Index);

	UFUNCTION()
	void OnChangeAntiAliasing(const int32& Index);

	UFUNCTION()
	void OnChangeAnisotropicFiltering(const int32& Index);

	UFUNCTION()
	void OnChangeMotionBlurQuality(const int32& Index);

	UFUNCTION()
	void OnChangeAmbientOcclusion(const int32& Index);

	UFUNCTION()
	void OnChangeDepthOfField(const int32& Index);

	UFUNCTION()
	void OnChangeBloom(const int32& Index);

	UFUNCTION()
	void OnChangeScreenSpaceReflections(const int32& Index);

	UFUNCTION()
	void OnChangeTessellation(const int32& Index);

	UFUNCTION()
	void OnChangeRayTracing(const bool& Value);
	
	void ApplySetting(TFunctionRef<void(UGameUserSettings*)> ApplyFunc);
};
