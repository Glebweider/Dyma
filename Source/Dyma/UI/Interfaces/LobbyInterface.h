// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LobbyInterface.generated.h"

class UBaseUserWidget;
// This class does not need to be modified.
UINTERFACE()
class ULobbyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DYMA_API ILobbyInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void StartGame();
};
