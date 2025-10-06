// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_PlayerState.h"

#include "Dyma/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "Net/UnrealNetwork.h"

void ADF_PlayerState::Server_SetVote_Implementation(APlayerState* TargetPlayer)
{
	VotedForPlayer = TargetPlayer;
}

void ADF_PlayerState::SetProject(const FProjectData& InProject)
{
	Project = InProject;
}

void ADF_PlayerState::OnRep_FaceRow()
{
	APawn* Pawn = GetPawn();
	if (!Pawn)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ADF_PlayerState::OnRep_FaceRow);
		return;
	}
	
	IToPlayerInterface::Execute_ApplyFaceByRow(Pawn, FaceRowName);
}

void ADF_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADF_PlayerState, VotedForPlayer);
	DOREPLIFETIME(ADF_PlayerState, bIsParticipant);
	DOREPLIFETIME(ADF_PlayerState, FaceRowName);
}
