// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DF_MainGamemode.generated.h"

enum class EGamePhase : uint8;
class ADF_GameState;
class AChair;
/**
 * 
 */
UCLASS()
class DUSTFALL_API ADF_MainGamemode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartGame();
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void StartDocReviewPhase();
	virtual void StartDocReviewPhaseDelayed();

protected:
	UFUNCTION()
	void StartRoundsPhase();

	UFUNCTION()
	void StartDebatPhase();

	UFUNCTION()
	void StartVotePhase();

	UFUNCTION()
	void CountVotesPhase();

	UPROPERTY()
	TArray<AChair*> Chairs;

	UPROPERTY()
	ADF_GameState* DF_GameState;

private:
	void NextSpeaker();
	void PauseBeforeNext();
	
	bool bIsLobbyOpen = true;
	
	int32 CurrentSpeakerIndex = 0;
	int32 CurrentRound = 0;
	
	TArray<ACharacter*> RoundCharacters;
	
	FTimerHandle SpeakerTimer;
	FTimerHandle PauseTimer;
};
