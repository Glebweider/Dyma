// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingItem_CheckBox.h"

#include "SettingItem_ComboBox.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

void USettingItem_CheckBox::NativeConstruct()
{
	Super::NativeConstruct();

	if (CheckBox)
	{
		CheckBox->OnCheckStateChanged.AddDynamic(this, &USettingItem_CheckBox::HandleCheckBoxChanged);
	}
}

void USettingItem_CheckBox::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Text_Title) Text_Title->SetText(FText::FromString(Title));
}

void USettingItem_CheckBox::SetValue(bool value)
{
	if (CheckBox) CheckBox->SetIsChecked(value);
}

bool USettingItem_CheckBox::GetValue()
{
	return CheckBox && CheckBox->IsChecked();
}

void USettingItem_CheckBox::HandleCheckBoxChanged(bool value)
{
	OnCheckBoxChanged.Broadcast(value);
}
