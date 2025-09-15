// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingItemWidget.h"
#include "SettingItem_ComboBox.generated.h"

/**
 * 
 */

class UComboBoxString;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboBoxSelectionChanged, const FString&, SelectedItem);

UCLASS()
class DUSTFALL_API USettingItem_ComboBox : public USettingItemWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnComboBoxSelectionChanged OnSelectionChanged;
	
	void SetItems(TArray<FString> ItemsSource);
	void SetDefaultValue(FString SelectedItem);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ComboBox;

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
private:
	UFUNCTION()
	void HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectInfo);
};
