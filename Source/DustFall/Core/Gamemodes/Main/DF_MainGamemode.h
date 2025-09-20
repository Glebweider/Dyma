// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DustFall/Core/Interface/GamemodeInterface.h"
#include "GameFramework/GameModeBase.h"
#include "DF_MainGamemode.generated.h"

enum class EGamePhase : uint8;
class ADF_GameState;
class AChair;
/**
 * 
 */
UCLASS()
class DUSTFALL_API ADF_MainGamemode : public AGameModeBase, public IGamemodeInterface
{
	GENERATED_BODY()

public:
	virtual void StartGame();
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void StartDocReviewPhase();
	virtual void StartDocReviewPhaseDelayed();
	virtual void AnvilOverlapPlayer_Implementation() override;

protected:
	UFUNCTION()
	void StartRoundsPhase();

	UFUNCTION()
	void StartDebatPhase();

	UFUNCTION()
	void StartVotePhase();

	UFUNCTION()
	void CountVotesPhase();

	UFUNCTION()
	void DelayedVotePhase();

	UPROPERTY()
	TArray<AChair*> Chairs;

	UPROPERTY()
	ADF_GameState* DF_GameState;

	UPROPERTY()
	ACharacter* KickedPlayer;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<AActor> AnvilClass;

private:
	void NextSpeaker();
	void PauseBeforeNext();
	
	bool bIsLobbyOpen = true;
	
	int32 CurrentSpeakerIndex;
	int32 CurrentRound;
	
	TArray<ACharacter*> RoundCharacters;
	
	FTimerHandle SpeakerTimer;
	FTimerHandle PauseTimer;
};
