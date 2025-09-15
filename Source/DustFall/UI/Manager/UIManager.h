// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DustFall/UI/Interface/PlayerToUIInterface.h"
#include "DustFall/UI/Widgets/BaseUserWidget/BaseUserWidget.h"
#include "UIManager.generated.h"


class UInputMappingContext;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUSTFALL_API UUIManager : public UActorComponent, public IPlayerToUIInterface
{
	GENERATED_BODY()

public:
	UUIManager();
	
	/** Interfaces */
	UBaseUserWidget* GetUI_Implementation(FName WidgetName) override;
	bool ShowUI_Implementation(TSubclassOf<UBaseUserWidget> WidgetClass) override;
	void HandleEscape_Implementation() override;
	void ChangeVisibilityWidgetByName_Implementation(FName WidgetName) override { ChangeVisibilityWidget(WidgetName); };

protected:
	virtual void BeginPlay() override;
	void SetInputSettings(bool bIsUIActive);
	void ChangeVisibilityWidget(FName WidgetName);
	UBaseUserWidget* GetActivityWidgetByClass(TSubclassOf<UBaseUserWidget> WidgetClass);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UBaseUserWidget> PauseMenuWidget;

	UPROPERTY()
	UBaseUserWidget* ActivityWidget;

	UPROPERTY()
	APlayerController* PlayerController;
	
	UPROPERTY()
	TMap<FName, UBaseUserWidget*> Widgets;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* MainInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* PauseInputMappingContext;
};
