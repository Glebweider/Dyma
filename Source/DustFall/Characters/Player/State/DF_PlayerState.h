// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DustFall/Core/Interface/GameStateInterface.h"
#include "DustFall/Core/Structures/Project.h"
#include "GameFramework/PlayerState.h"
#include "DF_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DUSTFALL_API ADF_PlayerState : public APlayerState, public IGameStateInterface
{
	GENERATED_BODY()

public:
	virtual FProjectData GetProjectData_Implementation() override { return Project; };
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Voting")
	APlayerState* VotedForPlayer = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_SetVote(APlayerState* TargetPlayer);

	UFUNCTION()
	void SetProject(const FProjectData& InProject);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Voting")
	FProjectData Project;
	TMap<FString, bool> ProjectFlags;
};
