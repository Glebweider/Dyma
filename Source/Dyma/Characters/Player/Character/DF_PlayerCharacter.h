// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dyma/Characters/Player/Interfaces/InputToPlayerInterface.h"
#include "Dyma/Characters/Player/Interfaces/ToPlayerInterface.h"
#include "GameFramework/Character.h"
#include "DF_PlayerCharacter.generated.h"

class UCameraComponent;
class UUIManager;
class UBaseUserWidget;
class UMaterialInstanceDynamic;
class APlayerController;
class APlayerState;

UCLASS()
class DYMA_API ADF_PlayerCharacter : public ACharacter, public IInputToPlayerInterface, public IToPlayerInterface
{
	GENERATED_BODY()

public:
	ADF_PlayerCharacter();
	
	virtual void StartVoteRound_Implementation() override;
	virtual void StartFinalVoteRound_Implementation() override;
	virtual void StopVoteRound_Implementation() override;
	virtual void KickedPlayerName_Implementation(const FString& PlayerName) override;
	virtual void ApplyFaceByRow_Implementation(FName RowName) override;
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
	void OnVotingTimer();

	UFUNCTION(NetMulticast, Reliable)
	void OnVotingFinalTimer();

	/** Multi RPC's */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_StopVote();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetMicrophoneActive(bool bIsActive);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetFaceRow(FName RowName);

	/** Server RPC's */
	UFUNCTION(Server, Reliable)
	void Server_SetMicrophoneActive(bool bIsActive);
	
	UFUNCTION(Server, Reliable)
	void Server_SetFaceRow(FName RowName);

	UFUNCTION(Server, Reliable)
	void Server_SetOwnerInteract(AActor* NewHitActor);

	UPROPERTY()
	UCameraComponent* CameraComponent;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UMaterialInstanceDynamic* CharacterMaterial;

	UPROPERTY(Replicated)
	UTexture2D* FaceOpenTexture;

	UPROPERTY(Replicated)
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