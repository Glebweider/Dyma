// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"

#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"


UUIManager::UUIManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UBaseUserWidget* UUIManager::GetUI_Implementation(FName WidgetName)
{
	if (UBaseUserWidget** FoundWidget = Widgets.Find(WidgetName))
		return *FoundWidget;

	return nullptr;
}

void UUIManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!GetOwner()) return;
	
	PlayerController = Cast<APlayerController>(GetOwner());
}

bool UUIManager::ShowUI_Implementation(TSubclassOf<UBaseUserWidget> WidgetClass)
{
	if (!WidgetClass) return false;
	
	if (!PlayerController)
		PlayerController = Cast<APlayerController>(GetOwner());
	
	if (UBaseUserWidget* Widget = GetActivityWidgetByClass(WidgetClass))
	{
		if (Widget->IsVisible())
		{
			if (Widget->bCanClose)
			{
				Widget->SetVisibility(ESlateVisibility::Collapsed);
				
				ActivityWidget = nullptr;
				SetInputSettings(false);
				
				return false;
			}
			
			return true;
		}

		Widget->SetVisibility(ESlateVisibility::Visible);
		if (Widget->bCanClose)
			ActivityWidget = Widget;

		if (Widget->bCanBlockedInput)
			SetInputSettings(true);
		
		return true;
	}
	
	auto NewWidget = CreateWidget<UBaseUserWidget>(PlayerController, WidgetClass);
	if (!NewWidget) return false;

	NewWidget->AddToViewport();
	Widgets.Add(NewWidget->WidgetName, NewWidget);

	if (NewWidget->bCanClose)
	{
		ActivityWidget = NewWidget;
	}

	if (NewWidget->bCanBlockedInput)
		SetInputSettings(true);
	
	return true;
}

void UUIManager::HandleEscape_Implementation()
{
	if (ActivityWidget && ActivityWidget->bCanClose)
	{
		ActivityWidget->Close();
		ActivityWidget->SetVisibility(ESlateVisibility::Collapsed);
		ActivityWidget = nullptr;
		SetInputSettings(false);
		return;
	}
	
	if (!PauseMenuWidget) return;

	UBaseUserWidget* PauseWidget = GetActivityWidgetByClass(PauseMenuWidget);

	if (PauseWidget)
	{
		bool bIsVisible = PauseWidget->IsVisible();
		PauseWidget->SetVisibility(bIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		ActivityWidget = bIsVisible ? nullptr : PauseWidget;
		SetInputSettings(!bIsVisible);
	}
	else
	{
		auto NewPauseWidget = CreateWidget<UBaseUserWidget>(PlayerController, PauseMenuWidget);
		if (NewPauseWidget)
		{
			NewPauseWidget->AddToViewport();
			Widgets.Add(NewPauseWidget->WidgetName, NewPauseWidget);
			ActivityWidget = NewPauseWidget;
			SetInputSettings(true);
		}
	}
}

void UUIManager::ChangeVisibilityWidget(FName WidgetName)
{
	if (UBaseUserWidget** FoundWidget = Widgets.Find(WidgetName))
	{
		if (IsValid(*FoundWidget))
		{
			bool bVisible = (*FoundWidget)->IsVisible();
			(*FoundWidget)->SetVisibility(bVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

			if ((*FoundWidget)->bCanBlockedInput)
				SetInputSettings(!bVisible);
		}
	}
}

UBaseUserWidget* UUIManager::GetActivityWidgetByClass(TSubclassOf<UBaseUserWidget> WidgetClass)
{
	for (const TPair<FName, UBaseUserWidget*>& Pair : Widgets)
	{
		UBaseUserWidget* Widget = Pair.Value;
		if (IsValid(Widget) && Widget->GetClass() == WidgetClass)
			return Widget;
	}
	
	return nullptr;
}

void UUIManager::SetInputSettings(bool bIsUIActive)
{
	if (!PlayerController) return;

	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (MainInputMappingContext && PauseInputMappingContext)
			{
				Subsystem->RemoveMappingContext(bIsUIActive ? MainInputMappingContext : PauseInputMappingContext);
				Subsystem->AddMappingContext(bIsUIActive ? PauseInputMappingContext : MainInputMappingContext, 0);
			}
		}
		PlayerController->bShowMouseCursor = bIsUIActive;
	}
}
