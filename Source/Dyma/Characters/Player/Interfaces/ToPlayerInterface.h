// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dyma/Core/Enums/SittingPoses.h"
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
class DYMA_API IToPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetSittingPoses(ESittingPoses NewPoses);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	bool GetIsSitting();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetAnimWinPose();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void StartVoteRound();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void StartFinalVoteRound();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void UpdateAnimSitting(bool bIsNewSitting);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void StopVoteRound();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void KickedPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void ApplyFaceByRow(FName RowName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void NotifyPauseVoteAvailable(int32 CountPlayers);
};
