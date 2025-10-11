// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_UserSettings.h"

UDF_UserSettings::UDF_UserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MasterVolume = 1.f;
	VoiceVolume = 1.f;
	SFXVolume = 1.f;
	AmbientVolume = 1.f;
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