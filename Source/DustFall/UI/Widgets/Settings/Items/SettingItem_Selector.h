// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingItemWidget.h"
#include "SettingItem_Selector.generated.h"

class UButton;
class UTextBlock;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionChanged, const int32&, Index);

/**
 * 
 */
UCLASS()
class DUSTFALL_API USettingItem_Selector : public USettingItemWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSelectionChanged OnSelectionChanged;
	
	void SetSelectedIndex(const int32 Index);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSelector;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonPreviousOption;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonNextOption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Selector")
	TArray<FString> Options;

	int32 CurrentIndex;

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

private:
	UFUNCTION()
	void OnNext();

	UFUNCTION()
	void OnPrevious();

	void UpdateTextSelector(int32 Index);
};
