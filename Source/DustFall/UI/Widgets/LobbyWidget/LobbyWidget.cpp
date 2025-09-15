// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "DustFall/Core/Gamemodes/Main/DF_MainGamemode.h"
#include "DustFall/UI/Widgets/FindedSessionWidget/FindedSessionWidget.h"
#include "Kismet/GameplayStatics.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainGameInstance = GetGameInstance<UDF_MainGameInstance>();
	
	if (Btn_ApplyCreateSession)
	{
		Btn_ApplyCreateSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnApplyCreateSessionClicked);
	}
	if (Btn_ExitSession)
	{
		Btn_ExitSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnExitSessionClicked);
	}
	if (WidgetSwitcher)
	{
		if (GetOwningPlayer()->GetLocalRole() == ROLE_Authority)
			WidgetSwitcher->SetActiveWidgetIndex(1);
	}
}

void ULobbyWidget::OnApplyCreateSessionClicked()
{
	if (ADF_MainGamemode* GM = Cast<ADF_MainGamemode>(UGameplayStatics::GetGameMode(this)))
		GM->StartGame();
}

void ULobbyWidget::OnExitSessionClicked()
{
	if (MainGameInstance)
		MainGameInstance->AdvancedDestroySession();
}
