// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_MainGamemode.h"
#include "HttpModule.h"
#include "Online.h"
#include "OnlineSubsystemUtils.h"
#include "Algo/Count.h"
#include "Dyma/Characters/Player/Controller/DF_PlayerController.h"
#include "Dyma/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "Dyma/Characters/Player/State/DF_PlayerState.h"
#include "Dyma/Core/GameInstance/DF_MainGameInstance.h"
#include "Dyma/Core/GameState/DF_GameState.h"
#include "Dyma/Core/Interface/GameInstanceInterface.h"
#include "Dyma/Core/Structures/Project.h"
#include "Dyma/World/Bench/Bench.h"
#include "Dyma/World/Chair/Chair.h"
#include "Dyma/World/Nameplate/Nameplate.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"


void ADF_MainGamemode::StartGame()
{
	bUseSeamlessTravel = false;
	bGameStarted = true;
	DF_GameState = GetGameState<ADF_GameState>();
	
	if (!DF_GameState) return;
	if (DF_GameState->Projects.Num() > 1)
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (auto PlayerPawn = It->Get()->GetCharacter())
				for (AChair* Chair : Chairs)
				{
					if (!Chair->Character)
					{
						Multi_UpdateNameplate(Chair, It->Get());
						
						Chair->Character = Cast<ACharacter>(PlayerPawn);
						Chair->SetOwner(It->Get());
						Chair->Character->SetPlayerState(It->Get()->PlayerState);

						Multi_Partipant(It->Get()->PlayerState, true);
						IToPlayerInterface::Execute_SetSittingPoses(Chair->Character, ESittingPoses::Default);
						IToPlayerInterface::Execute_UpdateAnimSitting(Chair->Character, true);

						FVector SeatPosition = Chair->GetActorLocation() + FVector(0.f, 0.f, 85.f);
						PlayerPawn->SetActorLocation(SeatPosition);

						FVector TableCenter = FVector(300.f, -390.f, 0.f);
						FRotator LookAtRotation = (TableCenter - SeatPosition).Rotation();
					
						FRotator NewRotation(0.f, LookAtRotation.Yaw, 0.f);
						PlayerPawn->SetActorRotation(NewRotation);

						PlayerPawn->GetCharacterMovement()->MovementMode = MOVE_None;

						break;
					}
				}
		}
	
	DF_GameState->Projects.Empty();
	DF_GameState->bCanVotePause = false;
	IGameInstanceInterface::Execute_SetSessionJoinAllowed(GetGameInstance(), false);

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
            if (!bWasSuccessful || !Resp.IsValid() || Resp->GetResponseCode() != 200) return;

            FString JsonStr = Resp->GetContentAsString();

            TSharedPtr<FJsonObject> RootObject;
            const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
        	
            if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid()) return;
        	
            FString Event;
            if (RootObject->TryGetStringField(TEXT("event"), Event))
                DF_GameState->CurrentEvent = Event;
        	
            const TArray<TSharedPtr<FJsonValue>>* DataArray;
            if (!RootObject->TryGetArrayField(TEXT("data"), DataArray)) return;
        	
            for (const TSharedPtr<FJsonValue>& Value : *DataArray)
            {
                if (!Value.IsValid()) continue;

                const TSharedPtr<FJsonObject> Obj = Value->AsObject();
                if (!Obj.IsValid()) continue;

                FProjectData Project;

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
                ADF_PlayerController* PC = Cast<ADF_PlayerController>(It->Get());
                if (!PC) continue;
            	
                if (DF_GameState->Projects.IsValidIndex(ProjectIndex))
                {
                    PC->Client_StartGame(DF_GameState->Projects[ProjectIndex], DF_GameState->CurrentEvent);
                    ProjectIndex++;
                }
            }
        }
    );
	
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
		DF_GameState->SetPhase(EGamePhase::DocReview, 10.f, this, FName("StartRoundsPhase"));
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
	{
		if (ACharacter* Character = Chair->Character)
			if (IPlayerStateInterface::Execute_GetIsParticipant(Character->GetPlayerState()))
				RoundCharacters.Add(Character);
	}
	
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
	
	DF_GameState->SetPhaseDuration(5.f); // 45
	DF_GameState->SetMoveForCharacter(Speaker);
	
	GetWorldTimerManager().SetTimer(SpeakerTimer, this,
		&ADF_MainGamemode::PauseBeforeNext, 5.f, false); // 45
}

void ADF_MainGamemode::PauseBeforeNext()
{
	DF_GameState->SetMoveForCharacter(nullptr);
	DF_GameState->SetPhaseDuration(5.f);
	
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
	float Time = RoundCharacters.Num() * 5.f; // 15
	DF_GameState->SetPhase(EGamePhase::Debate, Time, this, FName("StartVotePhase"));
}

void ADF_MainGamemode::StartVotePhase()
{
	DF_GameState->SetPhase(EGamePhase::Vote, 10.f, this, FName("CountVotesPhase")); // 30

	for (AChair* Chair : Chairs) {
		if (ACharacter* Character = Chair->Character)
			IToPlayerInterface::Execute_StartVoteRound(Character);
	}
}

void ADF_MainGamemode::StartFinalVotePhase()
{
	DF_GameState->SetPhase(EGamePhase::FinalVote, 15.f, this, FName("CountFinalVotesPhase")); //60
	
	for (ABench* Bench : Benches)
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
	
	for (ABench* Bench : Benches)
	{
		if (!Bench) continue;

		for (ACharacter* Character : Bench->GetOccupants())
		{
			if (!Character) continue;

			auto PS = Character->GetPlayerState<ADF_PlayerState>();
			if (!PS) continue;

			auto VotedPS = Cast<ADF_PlayerState>(PS->VotedForPlayer);
			if (!VotedPS) continue;

			int32& Count = VoteCounts.FindOrAdd(VotedPS);
			Count++;

			PS->VotedForPlayer = nullptr;
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
	
	ADF_PlayerState* EliminatedPlayer;
	if (Leaders.Num() == 1)
	{
		EliminatedPlayer = Leaders[0];
	} else if (Leaders.Num() > 1) {
		EliminatedPlayer = Leaders[FMath::RandRange(0, Leaders.Num() - 1)];
	} else {
		TArray<ADF_PlayerState*> AllPlayers;

		for (AChair* Chair : Chairs)
		{
			if (!Chair || !Chair->Character) continue;

			if (auto PS = Cast<ADF_PlayerState>(Chair->Character->GetPlayerState()))
				if (IPlayerStateInterface::Execute_GetIsParticipant(PS))
					AllPlayers.Add(PS);
		}
		
		if (AllPlayers.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("CountFinalVotesPhase: NO PLAYERS FOUND!"));
			return;
		}
		
		if (AllPlayers.Num() == 1)
		{
			EliminatedPlayer = AllPlayers[0];
		} else {
			EliminatedPlayer = AllPlayers[FMath::RandRange(0, AllPlayers.Num() - 1)];
		}
	}
	
	if (!EliminatedPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("CountFinalVotesPhase: FAILED TO CHOOSE ELIMINATED PLAYER! ABORT."));
		return;
	}
	
	ADF_PlayerState* WinnerPlayer = nullptr;
	for (ADF_PlayerState* PS : Leaders)
	{
		if (PS && PS != EliminatedPlayer)
		{
			WinnerPlayer = PS;
			break;
		}
	}
	
	if (!WinnerPlayer)
		for (AChair* Chair : Chairs)
		{
			if (!Chair || !Chair->Character) continue;

			auto PS = Cast<ADF_PlayerState>(Chair->Character->GetPlayerState());
			if (PS && IPlayerStateInterface::Execute_GetIsParticipant(PS))
				if (PS != EliminatedPlayer)
				{
					WinnerPlayer = PS;
					break;
				}
		}
	
	if (!WinnerPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("CountFinalVotesPhase: NO WINNER FOUND! ABORT."));
		return;
	}
	
	const auto World = GetWorld();
	if (!World) return;

	APlayerController* WinnerPC = WinnerPlayer->GetPlayerController();
	if (!WinnerPC || !WinnerPC->GetCharacter())
	{
		UE_LOG(LogTemp, Error, TEXT("WinnerPlayer has no controller or character!"));
		return;
	}

	DF_GameState->SetMoveForCharacter(WinnerPC->GetCharacter());
	DF_GameState->SetCurrentPhase(EGamePhase::Finished);
	
	TArray<AActor*> CameraActors;
	UGameplayStatics::GetAllActorsWithTag(World, TEXT("FinalCamera"), CameraActors);
	
	for (AChair* Chair : Chairs)
	{
		if (!Chair || !Chair->Character) continue;

		if (Chair->Character->GetPlayerState() == WinnerPlayer)
		{
			TArray<AActor*> TargetPointActors;
			UGameplayStatics::GetAllActorsWithTag(World, TEXT("FinalPlayerPoint"), TargetPointActors);

			if (TargetPointActors.Num() > 0)
			{
				IToPlayerInterface::Execute_UpdateAnimSitting(Chair->Character, false);
				IToPlayerInterface::Execute_SetSittingPoses(Chair->Character, ESittingPoses::Default);
				IToPlayerInterface::Execute_SetAnimWinPose(Chair->Character);
				
				FTransform TargetTransform = TargetPointActors[0]->GetActorTransform();
				FVector Location = TargetTransform.GetLocation();
				TargetTransform.SetLocation(FVector(Location.X, Location.Y, Location.Z + 89.f));

				Chair->Character->SetActorTransform(TargetTransform);
			}
		}
	}
	
	if (CameraActors.Num() > 0)
	{
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
			It->Get()->SetViewTargetWithBlend(CameraActors[0], 0.0f);
	}
	
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		[this]
		{
			DF_GameState->SetCurrentPhase(EGamePhase::NewLobby);

			FTimerHandle TH;
			GetWorld()->GetTimerManager().SetTimer(
				TH,
				this,
				&ADF_MainGamemode::StartNewLobby,
				1.f,
				false
			);
		},
		8.f,
		false
	);
}

void ADF_MainGamemode::StartNewLobby()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (auto PC = It->Get())
		{
			PC->SetViewTargetWithBlend(PC->GetCharacter(), 0.0f);
			PC->GetCharacter()->GetCharacterMovement()->MovementMode = MOVE_Walking;
		}
	}
	
	for (AChair* Chair : Chairs)
	{
		if (Chair->Character)
		{
			IToPlayerInterface::Execute_SetSittingPoses(Chair->Character, ESittingPoses::Default);
			IToPlayerInterface::Execute_UpdateAnimSitting(Chair->Character, false);
		}
		
		Multi_UpdateNameplate(Chair, nullptr);
		Chair->Character = nullptr;
		Chair->SetOwner(nullptr);
	}

	for (ABench* Bench : Benches)
	{
		for (ACharacter* Character : Bench->GetOccupants())
		{
			Bench->LeaveSeat(Character);
		}
	}
	
	DF_GameState->bCanVotePause = true;
	bUseSeamlessTravel = true;
	bGameStarted = false;
	
	IGameInstanceInterface::Execute_SetSessionJoinAllowed(GetGameInstance(), true);
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
				if (auto VotedPS = Cast<ADF_PlayerState>(PS->VotedForPlayer))
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
	} else if (Leaders.Num() > 1) {
		EliminatedPlayer = Leaders[FMath::RandRange(0, Leaders.Num() - 1)];
	}

	if (EliminatedPlayer)
	{
		KickedPlayer = Cast<ACharacter>(EliminatedPlayer->GetPawn());
		DF_GameState->SetMoveForCharacter(KickedPlayer);
		
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector PlayerLocation = EliminatedPlayer->GetPawn()->GetActorLocation();

		IToPlayerInterface::Execute_SetSittingPoses(KickedPlayer, ESittingPoses::Bench);
		
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
			Multi_Partipant(KickedPlayer->GetPlayerState(), false);
			
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

void ADF_MainGamemode::Multi_Partipant_Implementation(APlayerState* PS, bool newIsPartipant)
{
	IPlayerStateInterface::Execute_SetIsParticipant(PS, newIsPartipant);
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
			PC->ClientTravel(TEXT("/Game/Maps/Lobby"), TRAVEL_Absolute);
	
	if (!SessionVoiceInterface.IsValid())
		if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
		{
			TSharedPtr<IOnlineVoice, ESPMode::ThreadSafe> VoiceInterface = Subsystem->GetVoiceInterface();

			if (VoiceInterface.IsValid())
				SessionVoiceInterface = VoiceInterface;
		}
	
	TSharedPtr<const FUniqueNetId> NewId = NewPlayer->PlayerState->GetUniqueId().GetUniqueNetId();
	if (!NewId.IsValid()) return;
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* ExistingPC = It->Get();
		if (!ExistingPC || ExistingPC == NewPlayer) continue;

		TSharedPtr<const FUniqueNetId> ExistingId = ExistingPC->PlayerState->GetUniqueId().GetUniqueNetId();
		if (ExistingId.IsValid())
		{
			SessionVoiceInterface->RegisterRemoteTalker(*ExistingId);
			SessionVoiceInterface->RegisterRemoteTalker(*NewId);
		}
	}

	Chairs.Empty();
	Benches.Empty();
	
	TArray<AActor*> ChairActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChair::StaticClass(), ChairActors);

	for (AActor* ChairActor : ChairActors)
		if (auto Chair = Cast<AChair>(ChairActor))
			Chairs.Add(Chair);

	TArray<AActor*> BenchActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABench::StaticClass(), BenchActors);

	for (AActor* BenchActor : BenchActors)
		if (auto Bench = Cast<ABench>(BenchActor))
			Benches.Add(Bench);

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
					Cast<APlayerController>(NewPlayer)->SetViewTargetWithBlend(CameraActors[0], 0.0f);

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

	if (!SessionVoiceInterface.IsValid()) return;
	
	if (APlayerState* ExitingState = Exiting->PlayerState)
		if (TSharedPtr<const FUniqueNetId> ExitingId = ExitingState->GetUniqueId().GetUniqueNetId(); ExitingId.IsValid())
			SessionVoiceInterface->UnregisterRemoteTalker(*ExitingId);
	
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
