// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DLSSLibrary.h"
#include "ScreenSettingsWidget.generated.h"

class USettingItem_Slider;
class USettingItem_CheckBox;
class USettingItem_ComboBox;
/**
 * 
 */
UCLASS()
class DUSTFALL_API UScreenSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	const TArray<FString> FrameRates = {
		TEXT("30"),
		TEXT("60"),
		TEXT("90"),
		TEXT("120"),
		TEXT("144"),
		TEXT("240"),
		TEXT("360"),
		TEXT("Unlimited")
	};
	
public:
	virtual void NativeConstruct() override;

protected:
	/** Binds */
	UPROPERTY(meta = (BindWidget))
	USettingItem_ComboBox* ComboBox_Screen;
	
	UPROPERTY(meta = (BindWidget))
	USettingItem_ComboBox* ComboBox_ScreenResolution;

	UPROPERTY(meta = (BindWidget))
	USettingItem_ComboBox* ComboBox_ScreenMode;

	UPROPERTY(meta = (BindWidget))
	USettingItem_ComboBox* ComboBox_RefreshRate;
	
	UPROPERTY(meta = (BindWidget))
	USettingItem_ComboBox* ComboBox_Dlss_Quality;

	UPROPERTY(meta = (BindWidget))
	USettingItem_CheckBox* CheckBox_VSync;

	UPROPERTY(meta = (BindWidget))
	USettingItem_CheckBox* CheckBox_Dlss_Enable;

	UPROPERTY(meta = (BindWidget))
	USettingItem_CheckBox* CheckBox_Dlss_GenFrame;
	
	UPROPERTY(meta = (BindWidget))
	USettingItem_Slider* Slider_RenderScale;

	UPROPERTY(meta = (BindWidget))
	USettingItem_Slider* Slider_Dlss_Sharpness;

private:
	void InitScreenSettings();

	UFUNCTION()
	void OnScreenResolutionChanged(const FString& SelectedItem);

	UFUNCTION()
	void OnScreenModeChange(const FString& SelectedItem);

	UFUNCTION()
	void OnRefreshRateChange(const FString& SelectedItem);

	UFUNCTION()
	void OnRenderScaleChanged(const float Value);

	UFUNCTION()
	void OnVSyncChange(const bool Value);

	UFUNCTION()
	void OnMonitorChange(const FString& SelectedItem);

	static TArray<FString> GetConnectedMonitorsName();
	static TArray<FString> GetScreenResolutions();
	static TArray<FString> GetDlssModeDisplayNames();
	static bool GetDlssModeFromDisplayName(const FString& DisplayName, UDLSSMode& OutMode);
};
