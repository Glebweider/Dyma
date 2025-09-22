// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_MainGamemode.h"
#include "HttpModule.h"
#include "DustFall/Characters/Player/Controller/DF_PlayerController.h"
#include "DustFall/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "DustFall/Characters/Player/State/DF_PlayerState.h"
#include "DustFall/Core/GameState/DF_GameState.h"
#include "DustFall/Core/Structures/Project.h"
#include "DustFall/World/Bench/Bench.h"
#include "DustFall/World/Chair/Chair.h"
#include "DustFall/World/Nameplate/Nameplate.h"
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
	Request->ProcessRequest();
	
	Request->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Resp.IsValid()) return;
			
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Resp->GetContentAsString()), JsonArray))
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
}

void ADF_MainGamemode::StartDocReviewPhase()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ADF_MainGamemode::StartDocReviewPhaseDelayed,
		3.0f,
		false
	);
	
	if (DF_GameState) //180
		DF_GameState->SetPhase(EGamePhase::DocReview, 10.0f, this, FName("StartRoundsPhase"));
}

void ADF_MainGamemode::StartDocReviewPhaseDelayed()
{
	for (AChair* Chair : Chairs) {
		if (Chair->Character)
			Chair->StartGame();
	}
}

void ADF_MainGamemode::StartRoundsPhase()
{
	DF_GameState->SetCurrentPhase(EGamePhase::Round);
	DF_GameState->SetCurrentRound(CurrentRound);

	RoundCharacters.Empty();
	for (AChair* Chair : Chairs)
		if (ACharacter* Character = Chair->Character)
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

	for (AChair* Chair : Chairs)
		if (ACharacter* Character = Chair->Character)
			IToPlayerInterface::Execute_StartVoteRound(Character);
}

void ADF_MainGamemode::CountVotesPhase()
{
	DF_GameState->SetCurrentPhase(EGamePhase::Elimination);
	
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

	ADF_PlayerState* EliminatedPlayer;
	if (Leaders.Num() == 1)
		EliminatedPlayer = Leaders[0];
	else
		EliminatedPlayer = Leaders[FMath::RandRange(0, Leaders.Num() - 1)];

	if (EliminatedPlayer)
	{
		KickedPlayer = Cast<ACharacter>(EliminatedPlayer->GetPawn());
		
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector PlayerLocation = EliminatedPlayer->GetPawn()->GetActorLocation();
		
		FVector Location(PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z + 200.f);
		FRotator Rotation = FRotator::ZeroRotator;
		
		GetWorld()->SpawnActor<AActor>(AnvilClass, Location, Rotation, Params);
	}

	CurrentRound++;

	
}

void ADF_MainGamemode::AnvilOverlapPlayer_Implementation()
{
	if (!KickedPlayer) return;
	
	DF_GameState->SetMoveForCharacter(KickedPlayer);
	for (AChair* Chair : Chairs)
	{
		if (Chair && Chair->Character == KickedPlayer)
		{
			Chair->Destroy();
			Chairs.Remove(Chair);
			break;
		}
	}

	TArray<AActor*> BenchActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABench::StaticClass(), BenchActors);
	
	for (AActor* BenchActor : BenchActors)
	{
		if (ABench* Bench = Cast<ABench>(BenchActor))
			if (Bench->SeatPlayer(KickedPlayer))
				break;
	}
	
	FTimerHandle DelayHandle;
	GetWorldTimerManager().SetTimer(DelayHandle, this, &ADF_MainGamemode::StartRoundsPhase, 3.0f, false);
}

void ADF_MainGamemode::Multi_UpdateNameplate_Implementation(AChair* Chair, AController* NewPlayer)
{
	FString PlayerName = NewPlayer->PlayerState->GetPlayerName().Left(10);

	if (auto Nameplate = Chair->Nameplate)
	{
		Nameplate->Username = PlayerName;
		
		if (Nameplate->HasAuthority())
			Nameplate->RenderText(PlayerName);
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
		{
			if (!Chair->Character)
			{
				Chair->Character = Cast<ACharacter>(PlayerPawn);
				Chair->SetOwner(PlayerPawn->GetController());

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
