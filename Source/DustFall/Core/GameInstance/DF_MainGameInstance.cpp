// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_MainGameInstance.h"

#include "AdvancedSessionsLibrary.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "EndSessionCallbackProxy.h"
#include "FindSessionsCallbackProxyAdvanced.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"


void UDF_MainGameInstance::Init()
{
	Super::Init();

	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UDF_MainGameInstance::OnJoinSessionComplete);
		}
	}
}

void UDF_MainGameInstance::InitUniquePlayerId()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;

	FBPUniqueNetId BPUniqueNetId;
	UAdvancedSessionsLibrary::GetUniqueNetID(PlayerController, BPUniqueNetId);

	if (BPUniqueNetId.IsValid())
	{
		UniquePlayerNetId = UAdvancedSessionsLibrary::Conv_BPUniqueIDToUniqueNetIDRepl(BPUniqueNetId);
	}
}

void UDF_MainGameInstance::AdvancedCreateSession(const FString& SessionName)
{
	if (!SessionInterface.IsValid()) return;
	
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;
	
	TArray<FSessionPropertyKeyPair> ExtraSettings;
	FSessionPropertyKeyPair NameSetting;
	NameSetting.Key = "SESSION_NAME";
	NameSetting.Data.SetValue(SessionName);
	ExtraSettings.Add(NameSetting);

	CreateProxySession = UCreateSessionCallbackProxyAdvanced::CreateAdvancedSession(
		this,
		ExtraSettings,
		PlayerController,
		8,
		0,
		false,
		true,
		false,
		true,
		true,
		true,
		false,
		false,
		false,
		true,
		true,
		true
	);

	if (!CreateProxySession) return;

	CreateProxySession->OnSuccess.AddDynamic(this, &UDF_MainGameInstance::OnCreateSessionSuccess);
	CreateProxySession->OnFailure.AddDynamic(this, &UDF_MainGameInstance::OnCreateSessionFailure);
	CreateProxySession->Activate();
}

void UDF_MainGameInstance::AdvancedDestroySession()
{
	UE_LOG(LogTemp, Warning, TEXT("11111111111111111111"));
	if (!SessionInterface.IsValid()) return;

	UE_LOG(LogTemp, Warning, TEXT("22222222222222222222222"));
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		UE_LOG(LogTemp, Warning, TEXT("33333333333333333333333333"));
		UEndSessionCallbackProxy* EndSessionProxy = UEndSessionCallbackProxy::EndSession(this, UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (!EndSessionProxy) return;

		UE_LOG(LogTemp, Warning, TEXT("44444444444444444444444444"));
		EndSessionProxy->OnSuccess.AddDynamic(this, &UDF_MainGameInstance::OnDestroySessionSuccess);
		EndSessionProxy->OnFailure.AddDynamic(this, &UDF_MainGameInstance::OnDestroySessionFailure);
		EndSessionProxy->Activate();
	}
}

void UDF_MainGameInstance::OnDestroySessionSuccess()
{
	UE_LOG(LogTemp, Log, TEXT("Session successfully destroyed."));
	
	GetWorld()->ServerTravel(TEXT("/Game/Maps/LobbyMenu"), true);
}

void UDF_MainGameInstance::OnDestroySessionFailure()
{
	UE_LOG(LogTemp, Warning, TEXT("Failed to destroy session."));
}

UFindSessionsCallbackProxyAdvanced* UDF_MainGameInstance::AdvancedFindSessions(const FString& SessionName)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return nullptr;

	TArray<FSessionsSearchSetting> Filters;
	FSessionPropertyKeyPair NameProperty;
	NameProperty.Key = "SESSION_NAME";
	NameProperty.Data.SetValue(SessionName); 

	FSessionsSearchSetting NameFilter;
	NameFilter.ComparisonOp = EOnlineComparisonOpRedux::Equals;
	NameFilter.PropertyKeyPair = NameProperty;

	Filters.Add(NameFilter);
	
	return UFindSessionsCallbackProxyAdvanced::FindSessionsAdvanced(
		this,
		PlayerController,
		1000,
		false,
		EBPServerPresenceSearchType::AllServers,
		Filters,
		false,
		false,
		false,
		true,
		0);
}

void UDF_MainGameInstance::OnCreateSessionSuccess()
{
	GEngine->AddOnScreenDebugMessage(
	-1,
	15.0f,
	FColor::Green,
	TEXT("Открытие карты"));
	
	GetWorld()->ServerTravel(TEXT("/Game/Maps/Main?listen"), true);
}

void UDF_MainGameInstance::OnCreateSessionFailure()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		15.0f,
		FColor::Red,
		TEXT("Не удалось создать сессию."));
}

void UDF_MainGameInstance::AdvancedJoinSession(const FString& SessionName, const int32& SessionIndex)
{
	if (!SessionInterface.IsValid() || !OnlineSessionResults.IsValidIndex(SessionIndex) || !UniquePlayerNetId.IsValid()) return;

	OnJoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	bool bStarted = SessionInterface->JoinSession(*UniquePlayerNetId, FName(SessionName), OnlineSessionResults[SessionIndex]);
	if (!bStarted)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);
	}
}

void UDF_MainGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);

	if (!SessionInterface.IsValid()) return;

	FString ConnectString;
	if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			GEngine->AddOnScreenDebugMessage(
			-1,
			15.0f,
			FColor::Green,
			TEXT("Connect"));
			
			PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
		}
	}
}