// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HUDInterface.generated.h"

class UBaseUserWidget;
// This class does not need to be modified.
UINTERFACE()
class UHUDInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUSTFALL_API IHUDInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void UpdateMicrophoneState(bool bIsActive);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetHelpVoteTextVisible();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetVoteText(const FString& NewText);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetKickedPlayerName(const FString& PlayerName);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetCastVote(bool bIsVoteBtnPressed);
};
