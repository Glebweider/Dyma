// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_MainGamemode.h"
#include "DustFall/Characters/Player/Controller/DF_PlayerController.h"
#include "DustFall/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "DustFall/Core/GameState/DF_GameState.h"
#include "DustFall/World/Chair/Chair.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void ADF_MainGamemode::StartGame()
{
	bUseSeamlessTravel = false;
	bIsLobbyOpen = false;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		if (auto PC = Cast<ADF_PlayerController>(It->Get()))
			PC->ClientStartGame();
}

void ADF_MainGamemode::StartDocReviewPhase()
{
	if (DF_GameState = GetGameState<ADF_GameState>(); DF_GameState) //180
		DF_GameState->SetPhase(EGamePhase::DocReview, 10.0f, this, FName("StartRoundsPhase"));
}

void ADF_MainGamemode::StartRoundsPhase()
{
	DF_GameState->SetCurrentPhase(EGamePhase::Round);
	DF_GameState->SetCurrentRound(CurrentRound);

	RoundCharacters.Empty();
	for (AChair* Chair : Chairs)
		if (ACharacter* Character = Chair->GetCharacter())
			RoundCharacters.Add(Character);

	CurrentSpeakerIndex = 0;

	if (RoundCharacters.Num() > 0)
		NextSpeaker();
}

void ADF_MainGamemode::NextSpeaker()
{
	ACharacter* Speaker = RoundCharacters[CurrentSpeakerIndex];
	
	DF_GameState->SetPhaseDuration(6.f); // 30
	DF_GameState->SetMoveForCharacter(Speaker);
	
	GetWorldTimerManager().SetTimer(SpeakerTimer, this,
		&ADF_MainGamemode::PauseBeforeNext, 6.f, false); // 30
}

void ADF_MainGamemode::PauseBeforeNext()
{
	DF_GameState->SetPhaseDuration(5.f);
	DF_GameState->SetMoveForCharacter(nullptr);
	
	if (CurrentSpeakerIndex + 1 >= RoundCharacters.Num())
	{
		StartDebatPhase();
		return;
	}
	
	GetWorldTimerManager().SetTimer(PauseTimer, [this]()
	{
		CurrentSpeakerIndex++;
		NextSpeaker();
	}, 5.f, false);
}

void ADF_MainGamemode::StartDebatPhase()
{
	DF_GameState->SetPhase(EGamePhase::Debate, 15.0f, this, FName("StartVotePhase")); // 120
}

void ADF_MainGamemode::StartVotePhase()
{
	DF_GameState->SetPhase(EGamePhase::Vote, 30.0f, this, FName("StartRoundsPhase"));

	CurrentRound++;

	for (AChair* Chair : Chairs)
		if (ACharacter* Character = Chair->GetCharacter())
			IToPlayerInterface::Execute_StartVoteRound(Character);

	// ГОЛОСОВАНННННННИЕ
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
	{
		for (AChair* Chair : Chairs)
		{
			if (!Chair->GetCharacter())
			{
				Chair->SetCharacter(Cast<ACharacter>(PlayerPawn));

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
	}
}