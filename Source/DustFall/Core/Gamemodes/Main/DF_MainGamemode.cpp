// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_MainGamemode.h"

#include "DustFall/Characters/Player/Controller/DF_PlayerController.h"
#include "DustFall/Core/GameState/DF_GameState.h"
#include "DustFall/World/Chair/Chair.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void ADF_MainGamemode::StartGame()
{
	bUseSeamlessTravel = false;
	bIsLobbyOpen = false;
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		if (auto PC = Cast<ADF_PlayerController>(It->Get()))
			PC->ClientStartGame();
	
	if (DF_GameState = GetGameState<ADF_GameState>(); DF_GameState)
	{
		DF_GameState->CurrentPhase = EGamePhase::DocReview;
	}
}

void ADF_MainGamemode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	if (Chairs.Num() == 0)
	{
		TArray<AActor*> ChairActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChair::StaticClass(), ChairActors);

		for (AActor* ChairActor : ChairActors)
			if (AChair* Chair = Cast<AChair>(ChairActor))
				Chairs.Add(Chair);
	}

	RestartPlayer(NewPlayer);
	
	if (APawn* PlayerPawn = NewPlayer->GetPawn())
		for (AChair* Chair : Chairs)
			if (!Chair->GetOwner())
			{
				Chair->SetOwner(NewPlayer);
				
				FVector SeatPos = Chair->GetActorLocation() + FVector(0.f, 0.f, 65.f);
				PlayerPawn->SetActorLocation(SeatPos);
				
				FVector TableCenter = FVector(0.f, 0.f, SeatPos.Z);
				FRotator LookAtRotation = (TableCenter - SeatPos).Rotation();
				PlayerPawn->SetActorRotation(LookAtRotation);

				TArray<AActor*> CameraActors;
				UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("StartCamera"), CameraActors);
				
				if (CameraActors.Num() > 0)
				{
					APlayerController* PC = Cast<APlayerController>(NewPlayer);
					
					PC->SetViewTargetWithBlend(CameraActors[0], 0.0f);
				}
				return;
			}
}

void ADF_MainGamemode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	TArray<AActor*> ChairActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChair::StaticClass(), ChairActors);

	for (AActor* ChairActor : ChairActors)
	{
		if (AChair* Chair = Cast<AChair>(ChairActor))
		{
			if (AController* Controller = Cast<AController>(Chair->GetOwner()))
			{
				if (ACharacter* Character = Cast<ACharacter>(Controller->GetCharacter()))
				{
					FString Username = Character->GetPlayerState()->GetPlayerName();
					Chair->Server_UpdateNameplate(Username);
					DebugPrint(GetWorld(), "Chair");
				}
			}
		}
			
	}
	
	// Задаём таймер на небольшую задержку, например 0.5 секунды
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this, NewPlayer]()
	{
		
	}, 1.f, false); // 0.5 секунды, false = один раз
}

void ADF_MainGamemode::SetPhase(EGamePhase NewPhase, float Duration, const FTimerDelegate& NextPhaseCallback)
{
	if (DF_GameState)
		DF_GameState->CurrentPhase = NewPhase;

	GetWorldTimerManager().SetTimer(
		PhaseTimer,
		NextPhaseCallback,
		Duration,
		false
	);
}

void ADF_MainGamemode::DebugPrint(UWorld* World, const FString& Message)
{
	FString NetModeString;
	FColor Color = FColor::Green;
	if (World && World->GetNetMode() == NM_Client)
	{
		NetModeString = TEXT("[Client] ");
	}
	else if (World && World->GetNetMode() == NM_ListenServer)
	{
		NetModeString = TEXT("[ListenServer] ");
	}
	else if (World && World->GetNetMode() == NM_DedicatedServer)
	{
		NetModeString = TEXT("[DedicatedServer] ");
	}
	else
	{
		NetModeString = TEXT("[Standalone] ");
	}

	FString FinalMessage = NetModeString + Message;

	if (GEngine)
	{
		// на экран
		GEngine->AddOnScreenDebugMessage(
			-1,              // Key (-1 = всегда новая строка)
			5.f,             // Время отображения
			Color, 
			FinalMessage
		);

		// в лог
		UE_LOG(LogTemp, Warning, TEXT("%s"), *FinalMessage);
	}
}
