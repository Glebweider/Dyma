// Fill out your copyright notice in the Description page of Project Settings.

#include "ScreenSettingsWidget.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "DLSSLibrary.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GameFramework/GameUserSettings.h"
#include "../Items/SettingItem_CheckBox.h"
#include "../Items/SettingItem_ComboBox.h"
#include "../Items/SettingItem_Slider.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"

void UScreenSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitScreenSettings();
}

///
/// Start Screen Settings
///

void UScreenSettingsWidget::InitScreenSettings()
{
	UGameUserSettings* userSettings = GEngine->GetGameUserSettings();

	if (userSettings == nullptr) return;

	// Init List Monitors
	if (ComboBox_Screen)
	{
		TArray<FString> Monitors = GetConnectedMonitorsName();

		ComboBox_Screen->SetItems(Monitors);
		ComboBox_Screen->SetDefaultValue(Monitors.IsValidIndex(0) ? Monitors[0] : TEXT("Unknown"));
		ComboBox_Screen->OnSelectionChanged.AddDynamic(this, &UScreenSettingsWidget::OnMonitorChange);
	}

	// Init Screen Resolution
	if (ComboBox_ScreenResolution)
	{
		const TArray<FString> ScreenResolutions = GetScreenResolutions();
		const FIntPoint UserScreenResolution = userSettings->GetScreenResolution();

		if (ComboBox_ScreenResolution)
		{
			ComboBox_ScreenResolution->SetItems(ScreenResolutions);
			ComboBox_ScreenResolution->SetDefaultValue(
				FString::Printf(TEXT("%dx%d"), UserScreenResolution.X, UserScreenResolution.Y));
		}

		ComboBox_ScreenResolution->OnSelectionChanged.AddDynamic(
			this, &UScreenSettingsWidget::OnScreenResolutionChanged);
	}

	// Init Screen Mode
	if (ComboBox_ScreenMode)
	{
		const EWindowMode::Type UserWindowMode = userSettings->GetFullscreenMode();

		const TArray<FString> ScreenModes = {"Fullscreen", "Windowed", "Windowed Fullscreen"};

		ComboBox_ScreenMode->SetItems(ScreenModes);
		FString WindowMode;

		switch (UserWindowMode)
		{
		case EWindowMode::Windowed:
			WindowMode = TEXT("Windowed");
			break;
		case EWindowMode::Fullscreen:
			WindowMode = TEXT("Fullscreen");
			break;
		case EWindowMode::WindowedFullscreen:
			WindowMode = TEXT("Windowed Fullscreen");
			break;
		default: WindowMode = TEXT("Non selected");
		}

		ComboBox_ScreenMode->SetDefaultValue(WindowMode);

		ComboBox_ScreenMode->OnSelectionChanged.AddDynamic(this, &UScreenSettingsWidget::OnScreenModeChange);
	}

	// Init Refresh Rate
	if (ComboBox_RefreshRate)
	{
		const float CurrentFrateRate = userSettings->GetFrameRateLimit();

		ComboBox_RefreshRate->SetItems(FrameRates);

		const FString SelectedFrameRate = CurrentFrateRate == 0.0f
			                                  ? TEXT("Unlimited")
			                                  : FString::Printf(TEXT("%f"), CurrentFrateRate);

		ComboBox_RefreshRate->SetDefaultValue(SelectedFrameRate);

		ComboBox_RefreshRate->OnSelectionChanged.AddDynamic(this, &UScreenSettingsWidget::OnRefreshRateChange);
	}

	// Init VSync
	if (CheckBox_VSync)
	{
		CheckBox_VSync->SetValue(userSettings->IsVSyncEnabled());
		CheckBox_VSync->OnCheckBoxChanged.AddDynamic(this, &UScreenSettingsWidget::OnVSyncChange);
	}

	// Init Render Scale
	if (Slider_RenderScale)
	{
		Slider_RenderScale->SetMinValue(70.0f);
		Slider_RenderScale->SetMaxValue(200.0f);
		Slider_RenderScale->SetValue(userSettings->GetDefaultResolutionScale());
		Slider_RenderScale->OnValueChanged.AddDynamic(this, &UScreenSettingsWidget::OnRenderScaleChanged);
	}

	///
	///	DLSS Settings Init
	///	If the DLL libraries are not supported, do not initialize for optimization.
	///
	if (UDLSSLibrary::IsDLSSSupported())
	{
		if (CheckBox_Dlss_Enable) CheckBox_Dlss_Enable->SetValue(UDLSSLibrary::IsDLSSEnabled());
		if (ComboBox_Dlss_Quality)
		{
			const TArray<FString> DlssModes = GetDlssModeDisplayNames();

			ComboBox_Dlss_Quality->SetItems(DlssModes);
		}
		if (Slider_Dlss_Sharpness)
		{
			const float DlssSharpness = UDLSSLibrary::GetDLSSSharpness();
			Slider_Dlss_Sharpness->SetMinValue(0);
			Slider_Dlss_Sharpness->SetMaxValue(1);
			Slider_Dlss_Sharpness->SetValue(DlssSharpness);
		}
	}
	else
	{
		if (CheckBox_Dlss_Enable) CheckBox_Dlss_Enable->SetIsEnabled(false);
		if (ComboBox_Dlss_Quality) ComboBox_Dlss_Quality->SetIsEnabled(false);
		if (CheckBox_Dlss_GenFrame) CheckBox_Dlss_GenFrame->SetIsEnabled(false);
		if (Slider_Dlss_Sharpness) Slider_Dlss_Sharpness->SetIsEnabled(false);
	}
}

void UScreenSettingsWidget::OnScreenResolutionChanged(const FString& SelectedItem)
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();

	if (UserSettings == nullptr) return;

	FIntPoint Resolution;

	TArray<FString> Parts;
	SelectedItem.ParseIntoArray(Parts, TEXT("x"), true);

	if (Parts.Num() == 2)
	{
		Resolution.X = FCString::Atoi(*Parts[0]);
		Resolution.Y = FCString::Atoi(*Parts[1]);

		UserSettings->SetScreenResolution(Resolution);
		UserSettings->ApplySettings(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid resolution format: %s"), *SelectedItem);
		Resolution = FIntPoint(0, 0);
	}
}

void UScreenSettingsWidget::OnScreenModeChange(const FString& SelectedItem)
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();

	if (UserSettings == nullptr) return;

	if (SelectedItem == TEXT("Fullscreen"))
		UserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
	else if (SelectedItem == TEXT("Windowed"))
		UserSettings->SetFullscreenMode(EWindowMode::Windowed);
	else if (SelectedItem == TEXT("Windowed Fullscreen"))
		UserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);

	UserSettings->ApplySettings(false);
}

void UScreenSettingsWidget::OnRefreshRateChange(const FString& SelectedItem)
{
	if (UGameUserSettings* UserSettings = GEngine->GetGameUserSettings())
	{
		const bool bIsUnlimited = SelectedItem.Equals(TEXT("Unlimited"), ESearchCase::IgnoreCase);
		const float FrameRate = bIsUnlimited ? 0.f : FCString::Atof(*SelectedItem);

		UserSettings->SetFrameRateLimit(FrameRate);
		UserSettings->ApplySettings(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to access GameUserSettings"));
	}
}

void UScreenSettingsWidget::OnRenderScaleChanged(const float Value)
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();

	if (UserSettings == nullptr) return;

	UserSettings->SetResolutionScaleValueEx(Value);
	UserSettings->ApplySettings(false);
}

void UScreenSettingsWidget::OnVSyncChange(const bool Value)
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();

	if (UserSettings == nullptr) return;

	UserSettings->SetVSyncEnabled(Value);
	UserSettings->ApplySettings(false);
}

void UScreenSettingsWidget::OnMonitorChange(const FString& SelectedItem)
{
	if (!GEngine || !GEngine->GameViewport || !GEngine->GameViewport->GetWindow() || !GEngine->GetGameUserSettings())
		return;

	FDisplayMetrics DisplayMetrics;
	FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);

	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();

	if (UserSettings == nullptr) return;

	const EWindowMode::Type CurrentScreenMode = UserSettings->GetFullscreenMode();

	UserSettings->SetFullscreenMode(EWindowMode::Windowed);
	UserSettings->ApplySettings(false);

	for (const FMonitorInfo& Monitor : DisplayMetrics.MonitorInfo)
	{
		if (Monitor.Name == SelectedItem)
		{
			FVector2D ScreenPosition = FVector2D(Monitor.DisplayRect.Left, Monitor.DisplayRect.Top);
			GEngine->GameViewport->GetWindow()->MoveWindowTo(ScreenPosition);
			break;
		}
	}

	UserSettings->SetFullscreenMode(CurrentScreenMode);
	UserSettings->ApplySettings(false);
}

TArray<FString> UScreenSettingsWidget::GetConnectedMonitorsName()
{
	TArray<FString> Monitors;

	FDisplayMetrics DisplayMetrics;
	FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);

	for (const FMonitorInfo& Monitor : DisplayMetrics.MonitorInfo)
	{
		Monitors.Add(Monitor.Name);
	}

	return Monitors;
}

TArray<FString> UScreenSettingsWidget::GetScreenResolutions()
{
	TArray<FString> screenResolutions;
	FScreenResolutionArray resArray;

	if (RHIGetAvailableResolutions(resArray, true))
	{
		for (const FScreenResolutionRHI& res : resArray)
		{
			if (res.Height >= 600 && res.Width >= 800)
			{
				screenResolutions.Add(FString::Printf(TEXT("%dx%d"), res.Width, res.Height));
			}
		}
	}

	return screenResolutions;
}

TArray<FString> UScreenSettingsWidget::GetDlssModeDisplayNames()
{
	TArray<FString> names;
	if (UEnum* EnumPtr = StaticEnum<UDLSSMode>())
	{
		for (int32 i = 0; i < EnumPtr->NumEnums() - 1; i++)
		{
			names.Add(EnumPtr->GetDisplayNameTextByIndex(i).ToString());
		}
	}
	return names;
}

bool UScreenSettingsWidget::GetDlssModeFromDisplayName(const FString& DisplayName, UDLSSMode& OutMode)
{
	if (UEnum* EnumPtr = StaticEnum<UDLSSMode>())
	{
		for (int32 i = 0; i < EnumPtr->NumEnums() - 1; i++)
		{
			if (EnumPtr->GetDisplayNameTextByIndex(i).ToString().Equals(DisplayName))
			{
				OutMode = static_cast<UDLSSMode>(EnumPtr->GetValueByIndex(i));
				return true;
			}
		}
	}
	return false;
}
