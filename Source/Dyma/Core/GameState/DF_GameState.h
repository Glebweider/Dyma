// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dyma/Core/Enums/GamePhase.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameState.h"
#include "DF_GameState.generated.h"

struct FProjectData;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnPhaseChanged, EGamePhase, NewPhase, int32, RoundNumber, float, Duration, ACharacter*, MoveForCharacter);

UCLASS()
class DYMA_API ADF_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FProjectData> Projects;
	
	UPROPERTY(BlueprintAssignable, Category="Phase")
	FOnPhaseChanged OnPhaseChanged;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentPhase)
	EGamePhase CurrentPhase = EGamePhase::Lobby;
	
	UPROPERTY(ReplicatedUsing=OnRep_RoundNumber)
	int32 RoundNumber = 0;
	
	UPROPERTY(ReplicatedUsing=OnRep_PhaseDuration)
	float PhaseDuration;

	UPROPERTY(ReplicatedUsing=OnRep_MoveForCharacter)
	ACharacter* MoveForCharacter;
	
	UFUNCTION(BlueprintCallable, Category="Phase")
	void SetPhase(EGamePhase NewPhase, float Duration, UObject* TimerTarget, FName TimerFunctionName);

	UFUNCTION(BlueprintCallable, Category="Phase")
	void SetCurrentPhase(EGamePhase NewPhase);

	UFUNCTION(BlueprintCallable, Category="Phase")
	void SetCurrentRound(int32 NewRoundNumber);

	UFUNCTION(BlueprintCallable, Category="Phase")
	void SetPhaseDuration(float Duration);

	UFUNCTION(BlueprintCallable, Category="Phase")
	void SetMoveForCharacter(ACharacter* Character);

protected:
	FTimerHandle PhaseTimer;

	UFUNCTION()
	void OnRep_CurrentPhase();
	
	UFUNCTION()
	void OnRep_RoundNumber();
	
	UFUNCTION()
	void OnRep_PhaseDuration();

	UFUNCTION()
	void OnRep_MoveForCharacter();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};