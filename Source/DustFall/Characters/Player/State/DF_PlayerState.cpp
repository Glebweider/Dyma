// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_PlayerState.h"
#include "Net/UnrealNetwork.h"

void ADF_PlayerState::Server_SetVote_Implementation(APlayerState* TargetPlayer)
{
	VotedForPlayer = TargetPlayer;
}

void ADF_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADF_PlayerState, VotedForPlayer);
}