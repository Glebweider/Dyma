// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dyma/Characters/Player/Interfaces/PlayerStateInterface.h"
#include "Dyma/Core/Structures/Project.h"
#include "GameFramework/PlayerState.h"
#include "DF_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DYMA_API ADF_PlayerState : public APlayerState, public IPlayerStateInterface
{
	GENERATED_BODY()

public:
	virtual FProjectData GetProjectData_Implementation() override { return Project; };
	virtual bool GetIsParticipant_Implementation() override { return bIsParticipant;};
	virtual void SetIsParticipant_Implementation(bool bNewValue) override { bIsParticipant = bNewValue; };
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Voting")
	APlayerState* VotedForPlayer = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_FaceRow)
	FName FaceRowName;

	UFUNCTION(Server, Reliable)
	void Server_SetVote(APlayerState* TargetPlayer);

	UFUNCTION()
	void SetProject(const FProjectData& InProject);

	UFUNCTION()
	void OnRep_FaceRow();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	TMap<FString, bool> ProjectFlags;

	UPROPERTY(Replicated)
	bool bIsParticipant = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Voting")
	FProjectData Project;
};
