// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DustFall/Core/GameInstance/DF_MainGameInstance.h"
#include "DustFall/UI/Widgets/BaseUserWidget/BaseUserWidget.h"
#include "LobbyWidget.generated.h"

class UWidgetSwitcher;
class UButton;
/**
 * 
 */
UCLASS()
class DUSTFALL_API ULobbyWidget : public UBaseUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	/** Binds */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ApplyCreateSession;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ExitSession;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	/** Functions */
	UFUNCTION()
	void OnApplyCreateSessionClicked();

	UFUNCTION()
	void OnExitSessionClicked();

private:
	UPROPERTY()
	UDF_MainGameInstance* MainGameInstance;
};
