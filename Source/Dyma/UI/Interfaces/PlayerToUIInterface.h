// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerToUIInterface.generated.h"

class UBaseUserWidget;
// This class does not need to be modified.
UINTERFACE()
class UPlayerToUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DYMA_API IPlayerToUIInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	UBaseUserWidget* GetUI(FName WidgetName);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	bool ShowUI(TSubclassOf<UBaseUserWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void HandleEscape();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void ChangeVisibilityWidgetByName(FName WidgetName);
};
