// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DustFall/Characters/Player/Interfaces/InputToPlayerInterface.h"
#include "DustFall/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "GameFramework/Character.h"
#include "DF_PlayerCharacter.generated.h"

class UCameraComponent;
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
	virtual void StartFinalVoteRound_Implementation() override;
	virtual void StopVoteRound_Implementation() override;
	virtual void KickedPlayerName_Implementation(const FString& PlayerName) override;
	virtual void HandleMicrophone_Implementation(bool bIsNewMicrophone) override;
	virtual void HandleInteract_Implementation(bool bIsNewInteract) override;
	virtual void HandleZoom_Implementation(bool bIsNewZoom) override;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void OnVoteCast();
	virtual void RegisterRemoteTalker(APlayerState* RemotePlayerState);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void OnVotingTimer();

	UFUNCTION(NetMulticast, Reliable)
	virtual void OnVotingFinalTimer();

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multi_StopVote();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetMicrophoneActive(bool bIsActive);

	UFUNCTION(Server, Reliable)
	void Server_SetMicrophoneActive(bool bIsActive);

	UPROPERTY()
	UCameraComponent* CameraComponent;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UMaterialInstanceDynamic* CharacterMaterial;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	UTexture2D* FaceOpenTexture;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	UTexture2D* FaceCloseTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UBaseUserWidget> LobbyWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Face")
	UDataTable* FaceDataTable;

private:
	FTimerHandle VoteTimerHandle;
	FTimerHandle VoteCastTimerHandle;

	float TargetFov = 90.f;

	UPROPERTY()
	UUIManager* UIManager;
	
	UPROPERTY(Replicated)
	bool bHasVoted;

	UPROPERTY(Replicated)
	bool bIsCastingVote;
	
	UPROPERTY(Replicated)
	AActor* HitActor;
};