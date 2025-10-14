// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_MainGamemode.h"
#include "HttpModule.h"
#include "Algo/Count.h"
#include "Dyma/Characters/Player/Controller/DF_PlayerController.h"
#include "Dyma/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "Dyma/Characters/Player/State/DF_PlayerState.h"
#include "Dyma/Core/GameState/DF_GameState.h"
#include "Dyma/Core/Interface/GameInstanceInterface.h"
#include "Dyma/Core/Structures/Project.h"
#include "Dyma/World/Bench/Bench.h"
#include "Dyma/World/Chair/Chair.h"
#include "Dyma/World/Nameplate/Nameplate.h"
#include "GameFramework/Character.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"

void ADF_MainGamemode::StartGame()
{
	bUseSeamlessTravel = false;
	bGameStarted = true;
	DF_GameState = GetGameState<ADF_GameState>();

	if (!DF_GameState) return;

	IGameInstanceInterface::Execute_StartGame(GetGameInstance());

	int32 NumPlayers = GetWorld()->GetNumPlayerControllers();
	FString Url = FString::Printf(TEXT("https://swiftlylink.ru/dymax/generateTest?players=%d"), NumPlayers);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("User-Agent"), TEXT("UnrealEngine/5"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bWasSuccessful)
		{
			if (!bWasSuccessful) return;
			if (!Resp.IsValid()) return;
			if (Resp->GetResponseCode() != 200) return;

			TArray<TSharedPtr<FJsonValue>> JsonArray;
			FString JsonStr = Resp->GetContentAsString();

			if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonStr), JsonArray))
			{
				for (auto& JsonValue : JsonArray)
				{
					if (!JsonValue.IsValid()) continue;

					FProjectData Project;
					auto Obj = JsonValue->AsObject();

					Obj->TryGetStringField(TEXT("status"), Project.Status);
					Obj->TryGetStringField(TEXT("goal"), Project.Goal);
					Obj->TryGetStringField(TEXT("consequence"), Project.Consequence);
					Obj->TryGetStringField(TEXT("implementation"), Project.Implementation);
					Obj->TryGetStringField(TEXT("financing"), Project.Financing);
					Obj->TryGetStringField(TEXT("support"), Project.Support);

					DF_GameState->Projects.Add(Project);
					DF_GameState->bCanVotePause = true;
				}

				int32 ProjectIndex = 0;
				for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
				{
					if (auto PC = Cast<ADF_PlayerController>(It->Get()))
						if (DF_GameState->Projects.IsValidIndex(ProjectIndex))
						{
							PC->ClientStartGame(DF_GameState->Projects[ProjectIndex]);
							ProjectIndex++;
						}
				}
			}
		});

	Request->ProcessRequest();
}

void ADF_MainGamemode::StartDocReviewPhase()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ADF_MainGamemode::StartDocReviewPhaseDelayed,
		3.f,
		false
	);

	if (DF_GameState) //60
		DF_GameState->SetPhase(EGamePhase::DocReview, 6.f, this, FName("StartRoundsPhase"));
}

void ADF_MainGamemode::StartDocReviewPhaseDelayed()
{
	for (AChair* Chair : Chairs) {
		if (Chair->Character)
			Chair->Server_StartGame();
	}
}

void ADF_MainGamemode::StartVotePause_Implementation()
{
	if (!bPrevRoundIsPause)
		for (AChair* Chair : Chairs) {
			if (ACharacter* Character = Chair->Character)
			{
				int32 Count = Algo::CountIf(Chairs, [](AChair* Chair){ return Chair && Chair->Character; });
				
				IToPlayerInterface::Execute_NotifyPauseVoteAvailable(Character, Count);
			}
		}
}

void ADF_MainGamemode::StartRoundsPhase()
{
	RoundCharacters.Empty();
	for (AChair* Chair : Chairs)
		if (ACharacter* Character = Chair->Character)
			RoundCharacters.Add(Character);
	
	if (RoundCharacters.Num() == 2)
		return StartFinalVotePhase();
		
	DF_GameState->SetCurrentPhase(EGamePhase::Round);
	DF_GameState->SetCurrentRound(CurrentRound);

	CurrentSpeakerIndex = 0;

	if (RoundCharacters.Num() > 0)
		NextSpeaker();
}

void ADF_MainGamemode::NextSpeaker()
{
	ACharacter* Speaker = RoundCharacters[CurrentSpeakerIndex];
	
	DF_GameState->SetPhaseDuration(5.f); // 35
	DF_GameState->SetMoveForCharacter(Speaker);
	
	GetWorldTimerManager().SetTimer(SpeakerTimer, this,
		&ADF_MainGamemode::PauseBeforeNext, 5.f, false); // 35
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
	DF_GameState->SetPhase(EGamePhase::Debate, 10.f, this, FName("StartVotePhase")); // 90
}

void ADF_MainGamemode::StartVotePhase()
{
	DF_GameState->SetPhase(EGamePhase::Vote, 30.f, this, FName("CountVotesPhase"));

	for (AChair* Chair : Chairs) {
		if (ACharacter* Character = Chair->Character)
			IToPlayerInterface::Execute_StartVoteRound(Character);
	}
}

void ADF_MainGamemode::StartFinalVotePhase()
{
	DF_GameState->SetPhase(EGamePhase::FinalVote, 30.f, this, FName("CountFinalVotesPhase")); //30
	
	for (ABench* Bench : Benchs)
	{
		for (ACharacter* Character : Bench->GetOccupants())
		{
			if (Character)
				IToPlayerInterface::Execute_StartFinalVoteRound(Character);
		}
	}
}

void ADF_MainGamemode::CountFinalVotesPhase()
{
	TMap<ADF_PlayerState*, int32> VoteCounts;
	for (ABench* Bench : Benchs)
	{
		for (ACharacter* Character : Bench->GetOccupants())
		{
			if (!Character) continue;
			
			if (auto PS = Character->GetPlayerState<ADF_PlayerState>()) {
				if (ADF_PlayerState* VotedPS = Cast<ADF_PlayerState>(PS->VotedForPlayer))
				{
					int32& Count = VoteCounts.FindOrAdd(VotedPS);
					Count++;
					
					PS->VotedForPlayer = nullptr;
				}
			}
		}
	}
	
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
	else if (Leaders.Num() > 1)
	{
		EliminatedPlayer = Leaders[FMath::RandRange(0, Leaders.Num() - 1)];
	}
	
	if (EliminatedPlayer)
	{
		for (AChair* Chair : Chairs)
		{
			if (Chair->Character && Chair->Character->GetPlayerState() != EliminatedPlayer)
				UE_LOG(LogTemp, Warning, TEXT("Player state win %s"), *Chair->Character->GetPlayerState()->GetPlayerName());
		}
	}
}

void ADF_MainGamemode::CountVotesPhase()
{
	DF_GameState->SetCurrentPhase(EGamePhase::Elimination);
	CurrentRound++;

	if (bPrevRoundIsPause)
		bPrevRoundIsPause = false;
	
	TMap<ADF_PlayerState*, int32> VoteCounts;
	for (AChair* Chair : Chairs)
	{
		if (!Chair) continue;

		if (ACharacter* Character = Chair->Character)
			if (auto PS = Character->GetPlayerState<ADF_PlayerState>())
				if (ADF_PlayerState* VotedPS = Cast<ADF_PlayerState>(PS->VotedForPlayer))
				{
					int32& Count = VoteCounts.FindOrAdd(VotedPS);
					Count++;
					
					PS->VotedForPlayer = nullptr;
				}
	}

	if (VoteCounts.Num() == 0)
	{
		DF_GameState->SetMoveForCharacter(nullptr);
		
		for (AChair* Chair : Chairs) {
			if (ACharacter* Character = Chair->Character)
				IToPlayerInterface::Execute_StopVoteRound(Character);
		}
		
		FTimerHandle DelayHandle;
		GetWorldTimerManager().SetTimer(DelayHandle, this, &ADF_MainGamemode::StartRoundsPhase, 3.0f, false);
		
		return;
	};
	
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
	else if (Leaders.Num() > 1)
	{
		EliminatedPlayer = Leaders[FMath::RandRange(0, Leaders.Num() - 1)];
	}

	if (EliminatedPlayer)
	{
		KickedPlayer = Cast<ACharacter>(EliminatedPlayer->GetPawn());
		DF_GameState->SetMoveForCharacter(KickedPlayer);
		
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector PlayerLocation = EliminatedPlayer->GetPawn()->GetActorLocation();
		
		FVector Location(PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z + 200.f);
		FRotator Rotation = FRotator::ZeroRotator;
		
		GetWorld()->SpawnActor<AActor>(AnvilClass, Location, Rotation, Params);
	}
}

void ADF_MainGamemode::AnvilOverlapPlayer_Implementation()
{
	if (!KickedPlayer) return;
	
	for (AChair* Chair : Chairs) {
		if (ACharacter* Character = Chair->Character)
			IToPlayerInterface::Execute_StopVoteRound(Character);
	}
	
	for (int32 i = Chairs.Num() - 1; i >= 0; --i)
	{
		AChair* Chair = Chairs[i];
		if (Chair && Chair->Character == KickedPlayer)
		{
			Multi_Partipant(KickedPlayer->GetPlayerState());
			
			Chairs.RemoveAt(i);
			Chair->Destroy();
			break;
		}
	}

	TArray<AActor*> BenchActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABench::StaticClass(), BenchActors);
	
	for (AActor* BenchActor : BenchActors)
	{
		if (ABench* Bench = Cast<ABench>(BenchActor))
			if (Bench->SeatPlayer(KickedPlayer))
			{
				KickedPlayer = nullptr;
				break;
			}
	}
	
	FTimerHandle DelayHandle;
	GetWorldTimerManager().SetTimer(DelayHandle, this, &ADF_MainGamemode::StartRoundsPhase, 3.0f, false);
}

void ADF_MainGamemode::Multi_Partipant_Implementation(APlayerState* PS)
{
	IPlayerStateInterface::Execute_SetIsParticipant(PS, false);
}

void ADF_MainGamemode::Multi_UpdateNameplate_Implementation(AChair* Chair, AController* NewPlayer)
{
	if (!Chair || !Chair->Nameplate) return;

	if (!NewPlayer || !NewPlayer->PlayerState)
	{
		Chair->Nameplate->Username = TEXT("");
		
		if (Chair->Nameplate)
			Chair->Nameplate->RenderText(TEXT(""));
		
		return;
	}
	
	FString PlayerName = NewPlayer->PlayerState->GetPlayerName().Left(10);
	Chair->Nameplate->Username = PlayerName;

	if (Chair->Nameplate)
		Chair->Nameplate->RenderText(PlayerName);
}

void ADF_MainGamemode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	if (bGameStarted)
		if (APlayerController* PC = Cast<APlayerController>(NewPlayer))
			PC->ClientTravel(TEXT("/Game/Maps/LobbyMenu"), TRAVEL_Absolute);

	Chairs.Empty();
	Benchs.Empty();
	
	TArray<AActor*> ChairActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChair::StaticClass(), ChairActors);

	for (AActor* ChairActor : ChairActors)
		if (auto Chair = Cast<AChair>(ChairActor))
			Chairs.Add(Chair);

	TArray<AActor*> BenchActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABench::StaticClass(), BenchActors);

	for (AActor* BenchActor : BenchActors)
		if (auto Bench = Cast<ABench>(BenchActor))
			Benchs.Add(Bench);

	RestartPlayer(NewPlayer);

	if (APawn* PlayerPawn = NewPlayer->GetPawn())
		for (AChair* Chair : Chairs)
		{
			if (!Chair->Character)
			{
				Chair->Character = Cast<ACharacter>(PlayerPawn);
				Chair->SetOwner(PlayerPawn->GetController());
				Chair->Character->SetPlayerState(NewPlayer->PlayerState);

				FVector SeatPos = Chair->GetActorLocation() + FVector(0.f, 0.f, 85.f);
				PlayerPawn->SetActorLocation(SeatPos);

				FVector TableCenter = FVector(300.f, -390.f, 0.f);
				FRotator LookAtRotation = (TableCenter - SeatPos).Rotation();
				
				FRotator NewRotation(0.f, LookAtRotation.Yaw, 0.f);
				PlayerPawn->SetActorRotation(NewRotation);

				TArray<AActor*> CameraActors;
				UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("StartCamera"), CameraActors);

				if (CameraActors.Num() > 0)
				{
					auto PC = Cast<APlayerController>(NewPlayer);
					PC->SetViewTargetWithBlend(CameraActors[0], 0.0f);
				}

				return;
			}
		}
}

void ADF_MainGamemode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	TArray<AActor*> ChairActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChair::StaticClass(), ChairActors);

	for (AActor* Chair : ChairActors)
	{
		if (Chair->GetOwner() == NewPlayer)
			Multi_UpdateNameplate(Cast<AChair>(Chair), NewPlayer);
	}
}

void ADF_MainGamemode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	TArray<AActor*> ChairActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChair::StaticClass(), ChairActors);

	for (AActor* Chair : ChairActors)
	{
		if (Chair->GetOwner() == Exiting)
		{
			auto Chr = Cast<AChair>(Chair);
			
			Multi_UpdateNameplate(Chr, nullptr);
			Chr->SetOwner(nullptr);
			Chr->Character = nullptr;
		}
	}

	for (int32 i = RoundCharacters.Num() - 1; i >= 0; --i)
	{
		ACharacter* Char = RoundCharacters[i];
		if (Char && Char->GetController() == Exiting)
			RoundCharacters.RemoveAt(i);
	}
}
