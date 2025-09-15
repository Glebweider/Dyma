// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUSTFALL_API UBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool bCanClose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FName WidgetName;

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void Close();
};
