// Fill out your copyright notice in the Description page of Project Settings.


#include "BookSettingsWidget.h"

#include "Dyma/Core/UserSettings/DF_UserSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void UBookSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UserSettings = Cast<UDF_UserSettings>(GEngine->GetGameUserSettings());
	if (!UserSettings) return;

	UserSettings->LoadSettings();
	if (Selector_ScreenResolution)
	{
		const TArray<FString> ScreenResolutions = GetScreenResolutions();
		const FIntPoint UserScreenResolution = UserSettings->GetScreenResolution();

		Selector_ScreenResolution->SetOptions(ScreenResolutions);
		
		int32 CurrentIndex = ScreenResolutions.IndexOfByPredicate([&UserScreenResolution](const FString& ResString)
		{
			FString Left, Right;
			if (ResString.Split(TEXT("x"), &Left, &Right))
			{
				int32 X = FCString::Atoi(*Left);
				int32 Y = FCString::Atoi(*Right);
				return UserScreenResolution == FIntPoint(X, Y);
			}
			return false;
		});

		if (CurrentIndex != INDEX_NONE)
			Selector_ScreenResolution->SetSelectedIndex(CurrentIndex);

		Selector_ScreenResolution->OnSelectionChanged.AddDynamic(this, &UBookSettingsWidget::OnChangeSettings);

		InitialResolutionIndex = CurrentIndex;
	}

	if (Selector_ScreenMode)
	{
		EWindowMode::Type UserWindowMode = UserSettings->GetFullscreenMode();
		TArray<FString> ScreenModes = { TEXT("Полный"), TEXT("Оконный"), TEXT("Без рамки") };
		
		Selector_ScreenMode->SetOptions(ScreenModes);
		
		int32 WindowModeIndex;
		switch (UserWindowMode)
		{
		case EWindowMode::Windowed:
			WindowModeIndex = 1;
			break;
		case EWindowMode::Fullscreen:
			WindowModeIndex = 0;
			break;
		case EWindowMode::WindowedFullscreen:
			WindowModeIndex = 2;
			break;
		default:
			WindowModeIndex = 0;
			break;
		}
		
		Selector_ScreenMode->SetSelectedIndex(WindowModeIndex);
		Selector_ScreenMode->OnSelectionChanged.AddDynamic(this, &UBookSettingsWidget::OnChangeSettings);

		InitialWindowModeIndex = WindowModeIndex;
	}

	if (Selector_Graphics)
	{
		TArray<FString> GraphicsOptions = { TEXT("Низкие"), TEXT("Средние"), TEXT("Высокие"), TEXT("Бюрократическое") };
		Selector_Graphics->SetOptions(GraphicsOptions);

		int32 CurrentPreset = UserSettings->GetOverallScalabilityLevel();
		
		Selector_Graphics->SetSelectedIndex(CurrentPreset);
		Selector_Graphics->OnSelectionChanged.AddDynamic(this, &UBookSettingsWidget::OnChangeSettings);

		InitialGraphicsIndex = CurrentPreset;
	}
	
	// === Sounds ===
	if (Slider_MasterVolume)
	{
		Slider_MasterVolume->SetValue(UserSettings->GetMasterVolume());
		Slider_MasterVolume->OnValueChanged.AddDynamic(this, &UBookSettingsWidget::OnChangeSettingsVolume);
	}
	
	if (Slider_VoiceVolume)
	{
		Slider_VoiceVolume->SetValue(UserSettings->GetVoiceVolume());
		Slider_VoiceVolume->OnValueChanged.AddDynamic(this, &UBookSettingsWidget::OnChangeSettingsVolume);
	}
	
	if (Slider_SFXVolume)
	{
		Slider_SFXVolume->SetValue(UserSettings->GetSFXVolume());
		Slider_SFXVolume->OnValueChanged.AddDynamic(this, &UBookSettingsWidget::OnChangeSettingsVolume);
	}
	
	if (Slider_AmbientVolume)
	{
		Slider_AmbientVolume->SetValue(UserSettings->GetAmbientVolume());
		Slider_AmbientVolume->OnValueChanged.AddDynamic(this, &UBookSettingsWidget::OnChangeSettingsVolume);
	}
}

void UBookSettingsWidget::OnChangeSettings()
{
	bool bChanged =
		Selector_ScreenResolution->GetSelectedIndex() != InitialResolutionIndex ||
		Selector_ScreenMode->GetSelectedIndex() != InitialWindowModeIndex ||
		Selector_Graphics->GetSelectedIndex() != InitialGraphicsIndex;

	if (Border_ApplyButton)
		Border_ApplyButton->SetVisibility(bChanged ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UBookSettingsWidget::OnChangeSettingsVolume()
{
	if (!MainSoundMix) return;

	const auto UpdateVolume = [&](USoundClass* SoundClass, float NewValue, float (UDF_UserSettings::*Getter)() const, void (UDF_UserSettings::*Setter)(float))
	{
		if (!SoundClass) return;

		const float CurrentValue = (UserSettings->*Getter)();
		if (FMath::IsNearlyEqual(CurrentValue, NewValue, 0.001f))
			return;

		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MainSoundMix, SoundClass, NewValue, 1.f, true);
		(UserSettings->*Setter)(NewValue);
	};
	
	UpdateVolume(MasterVolumeClass, Slider_MasterVolume ? Slider_MasterVolume->GetValue() : UserSettings->GetMasterVolume(), &UDF_UserSettings::GetMasterVolume, &UDF_UserSettings::SetMasterVolume);
	UpdateVolume(VoiceVolumeClass, Slider_VoiceVolume ? Slider_VoiceVolume->GetValue() : UserSettings->GetVoiceVolume(), &UDF_UserSettings::GetVoiceVolume, &UDF_UserSettings::SetVoiceVolume);
	UpdateVolume(SFXVolumeClass, Slider_SFXVolume ? Slider_SFXVolume->GetValue() : UserSettings->GetSFXVolume(), &UDF_UserSettings::GetSFXVolume, &UDF_UserSettings::SetSFXVolume);
	UpdateVolume(AmbientVolumeClass, Slider_AmbientVolume ? Slider_AmbientVolume->GetValue() : UserSettings->GetAmbientVolume(), &UDF_UserSettings::GetAmbientVolume, &UDF_UserSettings::SetAmbientVolume);

	UGameplayStatics::PushSoundMixModifier(GetWorld(), MainSoundMix);
	
	GetWorld()->GetTimerManager().ClearTimer(SaveTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(SaveTimerHandle, this, &UBookSettingsWidget::SaveVolumeSettings, 0.5f, false);
}

void UBookSettingsWidget::SaveVolumeSettings()
{
	if (!UserSettings) return;
	UserSettings->ApplySettings(false);
	UserSettings->SaveSettings();
}

void UBookSettingsWidget::ApplySettings()
{
	int32 ResolutionIndex = Selector_ScreenResolution->GetSelectedIndex();
	int32 ModeIndex = Selector_ScreenMode->GetSelectedIndex();
	int32 GraphicsIndex = Selector_Graphics->GetSelectedIndex();

	// === Screen Resolution ===
	TArray<FString> ScreenResolutions = GetScreenResolutions();
	if (ScreenResolutions.IsValidIndex(ResolutionIndex))
	{
		FString Left, Right;
		if (ScreenResolutions[ResolutionIndex].Split(TEXT("x"), &Left, &Right))
		{
			int32 X = FCString::Atoi(*Left);
			int32 Y = FCString::Atoi(*Right);

			UserSettings->SetScreenResolution(FIntPoint(X, Y));
		}
	}

	// === Screen Mode ===
	EWindowMode::Type NewMode;
	switch (ModeIndex)
	{
		case 0: NewMode = EWindowMode::Fullscreen; break;
		case 1: NewMode = EWindowMode::Windowed; break;
		case 2: NewMode = EWindowMode::WindowedFullscreen; break;
		default: NewMode = EWindowMode::Fullscreen; break;
	}

	UserSettings->SetFullscreenMode(NewMode);

	// === Graphics Quality ===
	int32 QualityGraphicsLevel = FMath::Clamp(GraphicsIndex, 0, 3);
	UserSettings->SetOverallScalabilityLevel(QualityGraphicsLevel);

	// === Sounds ===
	/*float MasterVolume = UserSettings->GetMasterVolume();
	float VoiceVolume = UserSettings->GetVoiceVolume();
	float SFXVolume = UserSettings->GetSFXVolume();
	float AmbientVolume = UserSettings->GetAmbientVolume();
	
	if (!MainSoundMix) return;

	if (MasterVolumeClass) UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MainSoundMix, MasterVolumeClass, MasterVolume, 0.f, true);
	if (VoiceVolumeClass) UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MainSoundMix, VoiceVolumeClass, VoiceVolume, 0.f, true);
	if (SFXVolumeClass) UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MainSoundMix, SFXVolumeClass, SFXVolume, 0.f, true);
	if (AmbientVolumeClass) UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MainSoundMix, AmbientVolumeClass, AmbientVolume, 0.f, true);

	UGameplayStatics::PushSoundMixModifier(GetWorld(), MainSoundMix);
	
	UserSettings->SetMasterVolume(MasterVolume);
	UserSettings->SetVoiceVolume(VoiceVolume);
	UserSettings->SetSFXVolume(SFXVolume);
	UserSettings->SetAmbientVolume(AmbientVolume);*/

	// Save Settings
	UserSettings->ApplySettings(false);
	UserSettings->SaveSettings();

	InitialResolutionIndex = ResolutionIndex;
	InitialWindowModeIndex = ModeIndex;
	InitialGraphicsIndex = GraphicsIndex;

	if (Border_ApplyButton)
		Border_ApplyButton->SetVisibility(ESlateVisibility::Collapsed);
}

TArray<FString> UBookSettingsWidget::GetScreenResolutions()
{
	TArray<FString> screenResolutions;
	FScreenResolutionArray resArray;

	if (RHIGetAvailableResolutions(resArray, true))
		for (const FScreenResolutionRHI& res : resArray)
		{
			if (res.Height >= 600 && res.Width >= 800)
				screenResolutions.Add(FString::Printf(TEXT("%dx%d"), res.Width, res.Height));
		}

	return screenResolutions;
}