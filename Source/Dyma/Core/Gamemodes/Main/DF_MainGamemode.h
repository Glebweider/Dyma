// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Dyma/Core/Interface/GamemodeInterface.h"
#include "GameFramework/GameModeBase.h"
#include "DF_MainGamemode.generated.h"

class ABench;
class ATargetPoint;
enum class EGamePhase : uint8;
class ADF_GameState;
class AChair;
/**
 * 
 */
UCLASS()
class DYMA_API ADF_MainGamemode : public AGameModeBase, public IGamemodeInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void StartGame();
	virtual void StartDocReviewPhase();
	virtual void StartDocReviewPhaseDelayed();
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	/** Implements */
	virtual void AnvilOverlapPlayer_Implementation() override;
	virtual void StartVotePause_Implementation() override;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void StartRoundsPhase();

	UFUNCTION()
	void StartDebatPhase();

	UFUNCTION()
	void StartVotePhase();

	UFUNCTION()
	void StartFinalVotePhase();

	UFUNCTION()
	void StartNewLobby();

	UFUNCTION()
	void CountVotesPhase();

	UFUNCTION()
	void CountFinalVotesPhase();

	UFUNCTION(Reliable, NetMulticast)
	void Multi_Partipant(APlayerState* PS, bool newIsPartipant);

	UFUNCTION(Reliable, NetMulticast)
	void Multi_UpdateNameplate(AChair* Chair, AController* NewPlayer);

	UPROPERTY()
	TArray<AChair*> Chairs;

	UPROPERTY()
	TArray<ABench*> Benches;

	UPROPERTY()
	ADF_GameState* DF_GameState;

	UPROPERTY()
	ACharacter* KickedPlayer;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<AActor> AnvilClass;

private:
	IOnlineVoicePtr SessionVoiceInterface;
	
	void NextSpeaker();
	void PauseBeforeNext();

	bool bGameStarted = false;
	bool bPrevRoundIsPause = false;
	bool bNextRoundIsPause = false;
	
	int32 CurrentSpeakerIndex;
	int32 CurrentRound;
	
	TArray<ACharacter*> RoundCharacters;
	
	FTimerHandle SpeakerTimer;
	FTimerHandle PauseTimer;
};
