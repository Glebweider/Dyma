// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dyma/UI/Widgets/Settings/SettingItemWidget.h"
#include "SettingItem_Selector.generated.h"

class UButton;
class UTextBlock;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectionChanged);
/**
 * 
 */
UCLASS()
class DYMA_API USettingItem_Selector : public USettingItemWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSelectionChanged OnSelectionChanged;

	int32 GetSelectedIndex() { return CurrentIndex; }
	void SetSelectedIndex(const int32 Index);
	void SetOptions(const TArray<FString>& NewOptions) { Options = NewOptions; }

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Option;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_PrevOption;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_NextOption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Selector")
	TArray<FString> Options;

	int32 CurrentIndex;

private:
	UFUNCTION()
	void OnNext();

	UFUNCTION()
	void OnPrevious();

	void UpdateTextSelector(int32 Index);
};
