// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_UserSettings.h"

UDF_UserSettings::UDF_UserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MouseSensitivity = 1.f;
	MasterVolume = 1.f;
	VoiceVolume = 1.f;
	SFXVolume = 1.f;
	AmbientVolume = 1.f;
}

void UDF_UserSettings::SetMouseSensitivity(float value)
{
	MouseSensitivity = FMath::Clamp(value, 0.2f, 1.f);
}

void UDF_UserSettings::SetMasterVolume(float Value) 
{ 
	MasterVolume = FMath::Clamp(Value, 0.f, 1.f); 
}

void UDF_UserSettings::SetVoiceVolume(float Value) 
{ 
	VoiceVolume = FMath::Clamp(Value, 0.f, 1.f); 
}

void UDF_UserSettings::SetSFXVolume(float Value) 
{ 
	SFXVolume = FMath::Clamp(Value, 0.f, 1.f); 
}

void UDF_UserSettings::SetAmbientVolume(float Value) 
{ 
	AmbientVolume = FMath::Clamp(Value, 0.f, 1.f); 
}