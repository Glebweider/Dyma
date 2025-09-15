// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingItemWidget.h"
#include "SettingItem_CheckBox.generated.h"

class UCheckBox;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCheckBoxChanged, bool, value);

/**
 * 
 */
UCLASS()
class DUSTFALL_API USettingItem_CheckBox : public USettingItemWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCheckBoxChanged OnCheckBoxChanged;
	
	void SetValue(bool value);
	bool GetValue();
	
protected:
	UPROPERTY(meta = (BindWidget))
	UCheckBox* CheckBox;

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

private:
	UFUNCTION()
	void HandleCheckBoxChanged(bool value);
};
