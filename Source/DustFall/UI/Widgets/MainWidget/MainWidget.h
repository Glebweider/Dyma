// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxyAdvanced.h"
#include "DustFall/UI/Interface/PlayerToUIInterface.h"
#include "DustFall/UI/Widgets/BaseUserWidget/BaseUserWidget.h"
#include "MainWidget.generated.h"

class UVerticalBox;
class UFindedSessionWidget;
class UDF_MainGameInstance;
class UWidgetSwitcher;
class UEditableText;
class UCheckBox;
class UButton;
/**
 * 
 */
UCLASS()
class DUSTFALL_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	/** Binds */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ApplyCreateSession;

	UPROPERTY(meta = (BindWidget))
	UEditableText* EditText_CreateSessionName;

	UPROPERTY(meta = (BindWidget))
	UEditableText* EditText_FindSessionName;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WSwitch_FindSession;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VBox_FindedSessionsList;

	/** Functions */
	UFUNCTION(BlueprintCallable)
	void StartFindSessions();
	
	UFUNCTION()
	void OnFindSessionSuccess(const TArray<FBlueprintSessionResult>& Results);
	
	UFUNCTION()
	void OnFindSessionFailure(const TArray<FBlueprintSessionResult>& Results);
	
	UFUNCTION()
	void OnApplyCreateSessionClicked();

private:
	UPROPERTY()
	UDF_MainGameInstance* MainGameInstance;

	UPROPERTY()
	UFindSessionsCallbackProxyAdvanced* FindProxySession;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> FindedSessionWidgetClass;
};
