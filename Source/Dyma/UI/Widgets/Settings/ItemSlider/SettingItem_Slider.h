// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "Dyma/UI/Widgets/Settings/SettingItemWidget.h"
#include "SettingItem_Slider.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnValueChanged);
/**
 * 
 */
UCLASS()
class DYMA_API USettingItem_Slider : public USettingItemWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnValueChanged OnValueChanged;

	float GetValue() { return Slider ? Slider->GetValue() : 0.0f; }
	void SetValue(float value) { if (Slider) Slider->SetValue(value); }
	void SetMinValue(float value) { if (Slider) Slider->SetMinValue(value); }
	void SetMaxValue(float value) { if (Slider) Slider->SetMaxValue(value); }

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	USlider* Slider;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Option;

private:
	UFUNCTION()
	void HandleValueChanged(float value);
};
