// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DF_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DUSTFALL_API ADF_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Voting")
	APlayerState* VotedForPlayer = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_SetVote(APlayerState* TargetPlayer);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
