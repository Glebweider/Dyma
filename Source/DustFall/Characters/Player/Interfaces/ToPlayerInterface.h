// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ToPlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UToPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUSTFALL_API IToPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void StartVoteRound();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void StartFinalVoteRound();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void StopVoteRound();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void KickedPlayerName(const FString& PlayerName);
};
