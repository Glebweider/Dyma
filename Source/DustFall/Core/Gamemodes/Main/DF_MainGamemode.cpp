// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_MainGamemode.h"
#include "HttpModule.h"
#include "DustFall/Characters/Player/Controller/DF_PlayerController.h"
#include "DustFall/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "DustFall/Characters/Player/State/DF_PlayerState.h"
#include "DustFall/Core/GameState/DF_GameState.h"
#include "DustFall/Core/Structures/Project.h"
#include "DustFall/World/Chair/Chair.h"
#include "GameFramework/Character.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"

void ADF_MainGamemode::StartGame()
{
	bUseSeamlessTravel = false;
	bIsLobbyOpen = false;
	DF_GameState = GetGameState<ADF_GameState>();

	if (!DF_GameState) return;

	int32 NumPlayers = GetWorld()->GetNumPlayerControllers();
	FString Url = FString::Printf(TEXT("https://swiftlylink.ru/dymax/generate?players=%d"), NumPlayers);
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("User-Agent"), TEXT("UnrealEngine/5"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	
	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Resp.IsValid()) return;
		
		FString ResponseStr = Resp->GetContentAsString();
		
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
		TArray<TSharedPtr<FJsonValue>> JsonArray;

		if (FJsonSerializer::Deserialize(Reader, JsonArray) && JsonArray.Num() > 0)
		{
			for (auto& JsonValue : JsonArray)
			{
				if (!JsonValue.IsValid() || !JsonValue->AsObject().IsValid()) continue;

				TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();

				FProjectData Project;

				JsonObject->TryGetStringField(TEXT("status"), Project.Status);
				JsonObject->TryGetStringField(TEXT("goal"), Project.Goal);
				JsonObject->TryGetStringField(TEXT("consequence"), Project.Consequence);
				JsonObject->TryGetStringField(TEXT("implementation"), Project.Implementation);
				JsonObject->TryGetStringField(TEXT("financing"), Project.Financing);
				JsonObject->TryGetStringField(TEXT("support"), Project.Support);

				DF_GameState->Projects.Add(Project);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON array"));
		}
	});

	Request->ProcessRequest();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		if (auto PC = Cast<ADF_PlayerController>(It->Get()))
			PC->ClientStartGame();
}

void ADF_MainGamemode::StartDocReviewPhase()
{
	if (DF_GameState) //180
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
	DF_GameState->SetPhase(EGamePhase::Vote, 30.0f, this, FName("CountVotesPhase"));

	CurrentRound++;

	for (AChair* Chair : Chairs)
		if (ACharacter* Character = Chair->GetCharacter())
			IToPlayerInterface::Execute_StartVoteRound(Character);
}

void ADF_MainGamemode::CountVotesPhase()
{
	TMap<ADF_PlayerState*, int32> VoteCounts;
	
	for (AChair* Chair : Chairs)
	{
		if (!Chair) continue;

		if (ACharacter* Character = Chair->GetCharacter())
			if (auto PS = Character->GetPlayerState<ADF_PlayerState>())
				if (ADF_PlayerState* VotedPS = Cast<ADF_PlayerState>(PS->VotedForPlayer))
				{
					int32& Count = VoteCounts.FindOrAdd(VotedPS);
					Count++;
				}
	}

	if (VoteCounts.Num() == 0) return;
	
	int32 MaxVotes = 0;
	for (const auto& Pair : VoteCounts)
	{
		MaxVotes = FMath::Max(MaxVotes, Pair.Value);
	}
	
	TArray<ADF_PlayerState*> Leaders;
	for (const auto& Pair : VoteCounts)
	{
		if (Pair.Value == MaxVotes)
			Leaders.Add(Pair.Key);
	}

	ADF_PlayerState* EliminatedPlayer = nullptr;
	if (Leaders.Num() == 1)
	{
		EliminatedPlayer = Leaders[0];
	}
	else
	{
		int32 RandomIndex = FMath::RandRange(0, Leaders.Num() - 1);
		EliminatedPlayer = Leaders[RandomIndex];
	}

	if (EliminatedPlayer)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Исключён игрок: %s (голосов: %d)"),
			*EliminatedPlayer->GetPlayerName(),
			MaxVotes
		);
	}
	
	// DF_GameState->SetPhase(EGamePhase::Vote, 30.0f, this, FName("StartRoundsPhase"));
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