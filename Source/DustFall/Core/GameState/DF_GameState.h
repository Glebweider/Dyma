// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DustFall/Core/Enums/GamePhase.h"
#include "GameFramework/GameState.h"
#include "DF_GameState.generated.h"

/**
 * 
 */
UCLASS()
class DUSTFALL_API ADF_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game")
	EGamePhase CurrentPhase = EGamePhase::Lobby;
};
