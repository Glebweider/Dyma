// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingItemWidget.h"
#include "SettingItem_Slider.generated.h"

class USlider;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueChanged, float, value);

/**
 * 
 */
UCLASS()
class DUSTFALL_API USettingItem_Slider : public USettingItemWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnValueChanged OnValueChanged;
	
	void SetValue(float value);
	void SetMinValue(float value);
	void SetMaxValue(float value);
	float GetValue();

protected:
	UPROPERTY(meta = (BindWidget))
	USlider* Slider;

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

private:
	UFUNCTION()
	void HandleValueChanged(float value);
};
