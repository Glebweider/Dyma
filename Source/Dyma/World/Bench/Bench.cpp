// Fill out your copyright notice in the Description page of Project Settings.


#include "Bench.h"
#include "Dyma/Characters/Player/Interfaces/PlayerAnimInstanceInterface.h"
#include "GameFramework/Character.h"


ABench::ABench()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABench::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> Components;
	GetComponents<USceneComponent>(Components);

	for (USceneComponent* Comp : Components)
	{
		if (Comp && Comp->ComponentHasTag(TEXT("SeatPoint")))
		{
			SeatPoints.Add(Comp);
			Occupants.Add(nullptr);
		}
	}
}

bool ABench::SeatPlayer(ACharacter* Player)
{
	if (!Player) return false;
	
	for (int32 i = 0; i < SeatPoints.Num(); ++i)
	{
		if (Occupants[i] == nullptr)
		{
			FVector Location = SeatPoints[i]->GetComponentLocation();
			Player->SetActorLocation(FVector(Location.X, Location.Y, Location.Z + 90.f));

			Occupants[i] = Player;
			return true;
		}
	}
	
	return false;
}

void ABench::LeaveSeat(ACharacter* Player)
{
	if (!Player) return;

	IPlayerAnimInstanceInterface::Execute_SetIsSitting(Player->GetMesh()->GetAnimInstance(), false);

	for (int32 i = 0; i < Occupants.Num(); ++i)
	{
		if (Occupants[i] == Player)
		{
			Occupants[i] = nullptr;
			break;
		}
	}
}