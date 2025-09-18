// Fill out your copyright notice in the Description page of Project Settings.


#include "Chair.h"
#include "DustFall/World/Nameplate/Nameplate.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


AChair::AChair()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AChair::Server_UpdateNameplate_Implementation(const FString& PlayerName)
{
	Multicast_UpdateNameplate(PlayerName);
	UpdateNameplate(PlayerName);
}

void AChair::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AChair, Nameplate);
}

void AChair::Client_UpdateNameplate_Implementation(const FString& PlayerName)
{
	UpdateNameplate(PlayerName);
}

void AChair::UpdateNameplate(const FString& PlayerName)
{
	if (!Nameplate) return;

	Nameplate->RenderText(PlayerName);
}

void AChair::Multicast_UpdateNameplate_Implementation(const FString& PlayerName)
{
	Client_UpdateNameplate_Implementation(PlayerName);
	UpdateNameplate(PlayerName);
}
