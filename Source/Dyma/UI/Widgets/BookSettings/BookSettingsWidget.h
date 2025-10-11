// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Dyma/Core/UserSettings/DF_UserSettings.h"
#include "Dyma/UI/Widgets/Settings/ItemSelector/SettingItem_Selector.h"
#include "Dyma/UI/Widgets/Settings/ItemSlider/SettingItem_Slider.h"
#include "BookSettingsWidget.generated.h"

/**
 * 
 */
UCLASS()
class DYMA_API UBookSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pagination")
	int32 PageIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UBorder* Border_ApplyButton;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	USettingItem_Selector* Selector_ScreenResolution;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	USettingItem_Selector* Selector_ScreenMode;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	USettingItem_Selector* Selector_Graphics;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	USettingItem_Slider* Slider_MasterVolume;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	USettingItem_Slider* Slider_VoiceVolume;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	USettingItem_Slider* Slider_AmbientVolume;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	USettingItem_Slider* Slider_SFXVolume;

	// === Sounds ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundMix* MainSoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* MasterVolumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* VoiceVolumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* SFXVolumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* AmbientVolumeClass;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void ApplySettings();

private:
	TArray<FString> GetScreenResolutions();

	FTimerHandle SaveTimerHandle;
	
	int32 InitialResolutionIndex = 0;
	int32 InitialWindowModeIndex = 0;
	int32 InitialGraphicsIndex = 0;
	
	UPROPERTY()
	UDF_UserSettings* UserSettings;

	UFUNCTION()
	void OnChangeSettings();

	UFUNCTION()
	void OnChangeSettingsVolume();

	UFUNCTION()
	void SaveVolumeSettings();
};
