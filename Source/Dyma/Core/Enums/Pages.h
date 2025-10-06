#pragma once

UENUM(BlueprintType)
enum class EPages : uint8
{
	Main		   UMETA(DisplayName = "MainPage"),
	Settings       UMETA(DisplayName = "SettingsPage"),
	Customization  UMETA(DisplayName = "CustomizationPage")
};