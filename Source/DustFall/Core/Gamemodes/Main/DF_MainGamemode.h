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
	
protected:
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void SetPhase(EGamePhase NewPhase, float Duration, const FTimerDelegate& NextPhaseCallback);

private:
	FTimerHandle PhaseTimer;
	
	UPROPERTY()
	TArray<AChair*> Chairs;

	UPROPERTY()
	ADF_GameState* DF_GameState;

	bool bIsLobbyOpen = true;
};
