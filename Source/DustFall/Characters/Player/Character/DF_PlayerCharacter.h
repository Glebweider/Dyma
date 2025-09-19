// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DustFall/Characters/Player/Interfaces/InputToPlayerInterface.h"
#include "DustFall/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "GameFramework/Character.h"
#include "DF_PlayerCharacter.generated.h"

class UUIManager;
class UBaseUserWidget;
class UMaterialInstanceDynamic;
class APlayerController;
class APlayerState;

UCLASS()
class DUSTFALL_API ADF_PlayerCharacter : public ACharacter, public IInputToPlayerInterface, public IToPlayerInterface
{
	GENERATED_BODY()

public:
	ADF_PlayerCharacter();
	
	virtual void StartVoteRound_Implementation() override;
	
	virtual void HandleCrouch_Implementation(bool bIsNewCrouch) override;
	virtual void HandleMicrophone_Implementation(bool bIsNewMicrophone) override;
	virtual void HandleInteract_Implementation(bool bIsNewInteract) override;
	virtual void OnVoteCast();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	virtual void OnVotingTimer();

	UFUNCTION(BlueprintCallable, Category="Voice")
	void RegisterRemoteTalker(APlayerState* RemotePlayerState);

	UFUNCTION(Server, Reliable)
	void ServerSetMicrophoneActive(bool bIsActive);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetMicrophoneActive(bool bIsActive);
	
	UFUNCTION(Reliable, NetMulticast)
	virtual void Multi_StartVoteRound();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UBaseUserWidget> LobbyWidget;

	UPROPERTY()
	UMaterialInstanceDynamic* CharacterMaterial;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	UTexture2D* FaceOpenTexture;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	UTexture2D* FaceCloseTexture;

	UPROPERTY(EditDefaultsOnly, Category="Face")
	UDataTable* FaceDataTable;

private:
	bool bIsCastingVote;
	bool bHasVoted;
	FTimerHandle VoteTimerHandle;
	FTimerHandle VoteCastTimerHandle;
	
	UPROPERTY()
	AActor* HitActor;

	UPROPERTY()
	UUIManager* UIManager;
};