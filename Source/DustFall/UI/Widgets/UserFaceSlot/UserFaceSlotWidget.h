// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserFaceSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class DUSTFALL_API UUserFaceSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetRowName(const FName& InRowName);
	void SetImage(UTexture2D* Texture);

	UFUNCTION()
	void HandleFaceRowChanged(FName NewFaceRowName);

	UFUNCTION()
	void OnSelectFace();

protected:
	FName RowName;
	
	/** Binds */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Slot;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;

	UPROPERTY(meta = (BindWidget))
	UImage* Image;
};
