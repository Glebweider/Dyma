// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingItem_Slider.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"

void USettingItem_Slider::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Text_Title)
		Text_Title->SetText(FText::FromString(Title));
}

void USettingItem_Slider::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slider)
	{
		Slider->OnValueChanged.AddDynamic(this, &USettingItem_Slider::HandleValueChanged);

		if (Text_Option)
			Text_Option->SetText(FText::AsNumber(FMath::TruncToInt(Slider->GetValue() * 100)));
	}
}

void USettingItem_Slider::HandleValueChanged(float value)
{
	if (Text_Option)
		Text_Option->SetText(FText::AsNumber(FMath::TruncToInt(value * 100)));
	
	OnValueChanged.Broadcast();
}