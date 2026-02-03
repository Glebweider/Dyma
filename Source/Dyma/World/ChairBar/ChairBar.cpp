// Fill out your copyright notice in the Description page of Project Settings.


#include "ChairBar.h"
#include "Dyma/Characters/Player/Interfaces/PlayerStateInterface.h"
#include "Dyma/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AChairBar::AChairBar()
{
	PrimaryActorTick.bCanEverTick = false;
	Character = nullptr;
	bReplicates = true;
}

void AChairBar::StartInteract_Implementation(ACharacter* InteractPlayer)
{
	if (!HasAuthority())
	{
		ServerStartInteract(InteractPlayer);
		
		if (!IsValid(InteractPlayer)) return;
		if (Character != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SoundBusy, GetActorLocation());
			return;
		};
		
		if (IPlayerStateInterface::Execute_GetActorSeat(InteractPlayer->GetPlayerState())) return;
		
		IPlayerStateInterface::Execute_SetActorSeat(InteractPlayer->GetPlayerState(), this);
		IToPlayerInterface::Execute_UpdateAnimSitting(InteractPlayer, true);
		return;
	}
	
	HandleStartInteract(InteractPlayer);
}

void AChairBar::ServerStartInteract_Implementation(ACharacter* InteractPlayer)
{
	HandleStartInteract(InteractPlayer);
}

void AChairBar::HandleStartInteract(ACharacter* InteractPlayer)
{
	if (!IsValid(InteractPlayer)) return;
	
	if (Character)
	{
		if (SoundBusy)
			UGameplayStatics::PlaySoundAtLocation(this, SoundBusy, GetActorLocation());
		
		return;
	}
	
	if (IPlayerStateInterface::Execute_GetActorSeat(InteractPlayer->GetPlayerState())) return;
	
	Character = InteractPlayer;
	
	FVector Location = GetActorLocation();
	InteractPlayer->SetActorLocation(FVector(Location.X, Location.Y, Location.Z + 115.f));

	IPlayerStateInterface::Execute_SetActorSeat(InteractPlayer->GetPlayerState(), this);
	IToPlayerInterface::Execute_UpdateAnimSitting(InteractPlayer, true);

	if (auto Movement = InteractPlayer->FindComponentByClass<UCharacterMovementComponent>())
		Movement->DisableMovement();
}

void AChairBar::LeaveSeat_Implementation(ACharacter* Player)
{
	if (Player && Player->IsLocallyControlled())
		if (auto Movement = Player->FindComponentByClass<UCharacterMovementComponent>())
			Movement->SetMovementMode(MOVE_Walking);
	
	ServerLeaveSeat(Player);

	if (Player || Player == Character)
	{
		IPlayerStateInterface::Execute_SetActorSeat(
			Player->GetPlayerState(),
			nullptr
		);
		IToPlayerInterface::Execute_UpdateAnimSitting(Player, false);
	}
}

void AChairBar::ServerLeaveSeat_Implementation(ACharacter* Player)
{
	HandleLeaveSeat(Player);
}

void AChairBar::HandleLeaveSeat(ACharacter* Player)
{
	if (!Player || Player != Character)
		return;

	if (auto Movement = Player->FindComponentByClass<UCharacterMovementComponent>())
		Movement->SetMovementMode(MOVE_Walking);
	
	Character = nullptr;

	IPlayerStateInterface::Execute_SetActorSeat(
		Player->GetPlayerState(),
		nullptr
	);
	IToPlayerInterface::Execute_UpdateAnimSitting(Player, false);
	
	FVector Location = GetActorLocation();
	Player->SetActorLocation(FVector(Location.X, Location.Y, Location.Z + 140.f));
}

void AChairBar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChairBar, Character);
}
