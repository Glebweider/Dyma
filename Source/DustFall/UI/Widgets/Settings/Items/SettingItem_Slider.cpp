// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingItem_Slider.h"

#include "SettingItem_ComboBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void USettingItem_Slider::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slider)
	{
		Slider->OnValueChanged.AddDynamic(this, &USettingItem_Slider::HandleValueChanged);
	}
}

void USettingItem_Slider::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Text_Title)
	{
		Text_Title->SetText(FText::FromString(Title));
	}
}

void USettingItem_Slider::SetValue(float value)
{
	if (Slider) Slider->SetValue(value); 
}

void USettingItem_Slider::SetMinValue(float value)
{
	if (Slider) Slider->SetMinValue(value); 
}

void USettingItem_Slider::SetMaxValue(float value)
{
	if (Slider) Slider->SetMaxValue(value); 
}

float USettingItem_Slider::GetValue()
{
	return Slider ? Slider->GetValue() : 0.0f;
}

void USettingItem_Slider::HandleValueChanged(float value)
{
	OnValueChanged.Broadcast(value);
}
