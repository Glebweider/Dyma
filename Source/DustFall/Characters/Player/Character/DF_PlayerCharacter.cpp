// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_PlayerCharacter.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Blueprint/UserWidget.h"
#include "DustFall/UI/Manager/UIManager.h"
#include "Interfaces/VoiceInterface.h"
#include "Net/VoiceConfig.h"


class UBaseUserWidget;

ADF_PlayerCharacter::ADF_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADF_PlayerCharacter::HandleCrouch_Implementation(bool bIsNewCrouch)
{
	if (bIsNewCrouch)
	{
		Crouch();
	} else
	{
		UnCrouch();
	}
}

void ADF_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (auto UIManager = PC->FindComponentByClass<UUIManager>(); UIManager && LobbyWidget)
			IPlayerToUIInterface::Execute_ShowUI(UIManager, LobbyWidget);

		RegisterSteamRemoteTalker(PC->PlayerState);
	}
}


void ADF_PlayerCharacter::StartSteamVoice()
{
	if (IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld()))
		if (auto Voice = OSS->GetVoiceInterface())
		{
			Voice->RegisterLocalTalker(0);
			Voice->StartNetworkedVoice(0);
		}
}

void ADF_PlayerCharacter::StopSteamVoice()
{
	if (IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld()))
		if (auto Voice = OSS->GetVoiceInterface())
		{
			Voice->StopNetworkedVoice(0);
			Voice->UnregisterLocalTalker(0);
		}
}

void ADF_PlayerCharacter::RegisterSteamRemoteTalker(APlayerState* RemotePlayerState)
{
	if (!RemotePlayerState) return;

	if (IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld()))
		if (auto Voice = OSS->GetVoiceInterface())
		{
			TSharedPtr<const FUniqueNetId> RemoteId = RemotePlayerState->GetUniqueId().GetUniqueNetId();
			if (RemoteId.IsValid())
				Voice->RegisterRemoteTalker(*RemoteId);
		}
}