// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DustFall/UI/Interfaces/HUDInterface.h"
#include "DustFall/UI/Widgets/BaseUser/BaseUserWidget.h"
#include "DF_HUD.generated.h"

class UProgressBar;
class UImage;
enum class EGamePhase : uint8;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DUSTFALL_API UDF_HUD : public UBaseUserWidget, public IHUDInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	virtual void UpdateCountdown();  
	virtual void UpdateMicrophoneState_Implementation(bool bIsActive) override;
	virtual void SetHelpVoteTextVisible_Implementation() override;
	virtual void SetVoteText_Implementation(const FString& NewText) override;
	virtual void SetCastVote_Implementation(bool bIsVoteBtnPressed) override;
	virtual void UpdateVoteProgress();

	FTimerHandle CountdownTimerHandle;
	FTimerHandle VoteProgressTimer;
	float VoteProgressElapsed = 0.f;
	int32 RemainingSeconds = 0;
	
	/** Binds */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Phase;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_MoveFor;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Time;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Vote;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_HelpVote;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_Vote;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_Microphone;
	
	UFUNCTION()
	void OnPhaseChanged(EGamePhase NewPhase, int32 RoundNumber, float Duration, ACharacter* MoveForCharacter);
};
