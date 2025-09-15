// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "FindedSessionWidget.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class DUSTFALL_API UFindedSessionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	int32 SessionIndex;
	
	/** Binds */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_SessionName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_PlayerCounter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Ping;

protected:
	/** Binds */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_JoinSession;

	UFUNCTION()
	void OnJoinSessionClicked();

private:
	FTimerHandle ClickTimerHandle;
	double LastClickTime = 0.0;
	float DoubleClickThreshold = 0.25f;
};