// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_GameState.h"
#include "Net/UnrealNetwork.h"


void ADF_GameState::SetPhase(EGamePhase NewPhase, float Duration, UObject* TimerTarget, FName TimerFunctionName)
{
    CurrentPhase = NewPhase;
    PhaseDuration = Duration;
    
    OnPhaseChanged.Broadcast(CurrentPhase, RoundNumber, Duration, MoveForCharacter);

    GetWorldTimerManager().ClearTimer(PhaseTimer);
    
    FTimerDelegate TimerDel;
    TimerDel.BindUFunction(TimerTarget, TimerFunctionName);
    GetWorldTimerManager().SetTimer(PhaseTimer, TimerDel, Duration, false);
}

void ADF_GameState::SetCurrentPhase(EGamePhase NewPhase)
{
    CurrentPhase = NewPhase;
    OnPhaseChanged.Broadcast(NewPhase, RoundNumber, PhaseDuration, MoveForCharacter);
}

void ADF_GameState::SetCurrentRound(int32 NewRoundNumber)
{
    RoundNumber = NewRoundNumber;
    OnPhaseChanged.Broadcast(CurrentPhase, NewRoundNumber, PhaseDuration, MoveForCharacter);
}

void ADF_GameState::SetPhaseDuration(float Duration)
{
    PhaseDuration = Duration;
    OnPhaseChanged.Broadcast(CurrentPhase, RoundNumber, Duration, MoveForCharacter);
}

void ADF_GameState::SetMoveForCharacter(ACharacter* Character)
{
    MoveForCharacter = Character;
    OnPhaseChanged.Broadcast(CurrentPhase, RoundNumber, PhaseDuration, Character);
}

void ADF_GameState::OnRep_CurrentPhase()
{
    OnPhaseChanged.Broadcast(CurrentPhase, RoundNumber, PhaseDuration, MoveForCharacter);
}

void ADF_GameState::OnRep_RoundNumber()
{
    OnPhaseChanged.Broadcast(CurrentPhase, RoundNumber, PhaseDuration, MoveForCharacter);
}

void ADF_GameState::OnRep_PhaseDuration()
{
    OnPhaseChanged.Broadcast(CurrentPhase, RoundNumber, PhaseDuration, MoveForCharacter);
}

void ADF_GameState::OnRep_MoveForCharacter()
{
    OnPhaseChanged.Broadcast(CurrentPhase, RoundNumber, PhaseDuration, MoveForCharacter);
}

void ADF_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ADF_GameState, CurrentPhase);
    DOREPLIFETIME(ADF_GameState, RoundNumber);
    DOREPLIFETIME(ADF_GameState, PhaseDuration);
    DOREPLIFETIME(ADF_GameState, MoveForCharacter);
}
