// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingItem_Selector.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USettingItem_Selector::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Text_Title)
		Text_Title->SetText(FText::FromString(Title));
}

void USettingItem_Selector::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_PrevOption)
		Btn_PrevOption->OnClicked.AddDynamic(this, &USettingItem_Selector::OnPrevious);

	if (Btn_NextOption)
		Btn_NextOption->OnClicked.AddDynamic(this, &USettingItem_Selector::OnNext);
}

void USettingItem_Selector::SetSelectedIndex(const int32 Index)
{
	CurrentIndex = Index;
	UpdateTextSelector(CurrentIndex);
}

void USettingItem_Selector::OnNext()
{
	CurrentIndex = (CurrentIndex + 1) % Options.Num();
	OnSelectionChanged.Broadcast();
	UpdateTextSelector(CurrentIndex);
}

void USettingItem_Selector::OnPrevious()
{
	CurrentIndex = (CurrentIndex - 1 + Options.Num()) % Options.Num();
	OnSelectionChanged.Broadcast();
	UpdateTextSelector(CurrentIndex);
}

void USettingItem_Selector::UpdateTextSelector(int32 Index)
{
	if (Text_Option && Options.Num() > Index && Index >= 0)
		Text_Option->SetText(FText::FromString(Options[Index]));
}