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

void AChair::StartGame()
{

}

void AChair::SetCharacter(ACharacter* NewCharacter)
{
	Character = NewCharacter;
	SetOwner(NewCharacter->GetController());
		
	Server_UpdateNameplate(NewCharacter);
}

void AChair::OnRep_Character()
{
	Server_UpdateNameplate(Character);
}

void AChair::Server_UpdateNameplate_Implementation(ACharacter* NewCharacter)
{
	UpdateNameplate(NewCharacter);
}

void AChair::UpdateNameplate_Implementation(ACharacter* NewCharacter)
{
	if (!Nameplate) return;

	if (NewCharacter && NewCharacter->GetPlayerState())
		Nameplate->RenderText(NewCharacter->GetPlayerState()->GetPlayerName());
}

void AChair::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChair, Character);
	DOREPLIFETIME(AChair, Nameplate);
	DOREPLIFETIME(AChair, Book);
}