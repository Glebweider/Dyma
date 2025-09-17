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

ACharacter* AChair::GetCharacter()
{
	if (!Character) return nullptr;
	return Character;
}

void AChair::SetCharacter(ACharacter* NewCharacter)
{
	Character = NewCharacter;
	SetOwner(NewCharacter->GetController());
}

void AChair::OnRep_Character()
{
	
}

void AChair::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChair, Character);
}