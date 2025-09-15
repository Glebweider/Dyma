// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingItem_ComboBox.h"

#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"

void USettingItem_ComboBox::NativeConstruct()
{
	Super::NativeConstruct();

	if (ComboBox)
	{
		ComboBox->OnSelectionChanged.AddDynamic(this, &USettingItem_ComboBox::HandleSelectionChanged);
	}
}

void USettingItem_ComboBox::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Text_Title)
	{
		Text_Title->SetText(FText::FromString(Title));
	}
}

void USettingItem_ComboBox::SetItems(TArray<FString> ItemsSource)
{
	if (ComboBox)
	{
		for (const FString& Item : ItemsSource)
		{
			ComboBox->AddOption(Item);
		}
	}
}

void USettingItem_ComboBox::SetDefaultValue(FString SelectedItem)
{
	if (ComboBox)
	{
		ComboBox->SetSelectedOption(SelectedItem);
	}
}

void USettingItem_ComboBox::HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectInfo)
{
	OnSelectionChanged.Broadcast(SelectedItem);
}
