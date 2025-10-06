// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Dyma/Core/Gamemodes/Main/DF_MainGamemode.h"
#include "Dyma/UI/Manager/UIManager.h"
#include "Dyma/UI/Widgets/FindedSession/FindedSessionWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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
		if (const int32 NumPlayers = GM->GetNumPlayers(); NumPlayers > 1)
		{
			Btn_ApplyCreateSession->SetIsEnabled(false);
			GM->StartGame();
		}
}

void ULobbyWidget::OnExitSessionClicked()
{
	if (MainGameInstance)
		MainGameInstance->AdvancedDestroySession();
}

void ULobbyWidget::EndStartGame()
{
	if (auto UIManager = GetOwningPlayer()->FindComponentByClass<UUIManager>())
	{
		if (HUDWidget) 
			IPlayerToUIInterface::Execute_ShowUI(UIManager, HUDWidget);

		IPlayerToUIInterface::Execute_ChangeVisibilityWidgetByName(UIManager, "Lobby Menu");
	}

	if (ACharacter* MyPawn = Cast<ACharacter>(GetOwningPlayerPawn())) {
		GetOwningPlayer()->SetViewTarget(MyPawn);
		MyPawn->GetCharacterMovement()->DisableMovement();
	}

	if (GetOwningPlayerPawn()->HasAuthority())
		if (ADF_MainGamemode* GM = Cast<ADF_MainGamemode>(UGameplayStatics::GetGameMode(this)))
			GM->StartDocReviewPhase();
}
