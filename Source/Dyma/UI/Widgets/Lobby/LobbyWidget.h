// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dyma/Core/GameInstance/DF_MainGameInstance.h"
#include "Dyma/UI/Interfaces/LobbyInterface.h"
#include "Dyma/UI/Widgets/BaseUser/BaseUserWidget.h"
#include "LobbyWidget.generated.h"

class UWidgetSwitcher;
class UWidgetAnimation;
class UButton;
/**
 * 
 */
UCLASS()
class DYMA_API ULobbyWidget : public UBaseUserWidget, public ILobbyInterface
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

	UFUNCTION(BlueprintCallable, Category="LobbyWidget")
	void EndStartGame();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UBaseUserWidget> HUDWidget;

private:
	UPROPERTY()
	UDF_MainGameInstance* MainGameInstance;
};
