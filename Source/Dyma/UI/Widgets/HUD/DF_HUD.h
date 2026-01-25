// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dyma/UI/Interfaces/HUDInterface.h"
#include "Dyma/UI/Widgets/BaseUser/BaseUserWidget.h"
#include "DF_HUD.generated.h"

class UBorder;
class UTextBlock;
class UVerticalBox;
class UProgressBar;
class UImage;
enum class EGamePhase : uint8;
/**
 * 
 */
UCLASS()
class DYMA_API UDF_HUD : public UBaseUserWidget, public IHUDInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** Interfaces */
	virtual void UpdateMicrophoneState_Implementation(bool bIsActive) override;
	virtual void SetHelpVoteTextVisible_Implementation() override;
	virtual void SetVoteText_Implementation(const FString& NewText) override;
	virtual void SetCastVote_Implementation(bool bIsVoteBtnPressed) override;
	virtual void SetKickedPlayerName_Implementation(const FString& PlayerName) override;

protected:
	virtual void UpdateCountdown();  
	virtual void UpdateVoteProgress();

	UFUNCTION(BlueprintCallable)
	virtual void EndStartGame();

	FTimerHandle CountdownTimerHandle;
	FTimerHandle VoteProgressTimer;
	float VoteProgressElapsed = 0.f;
	int32 RemainingSeconds = 0;
	
	/** Binds */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Phase;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_TempPhase;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_MoveFor;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Time;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Vote;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_HelpVote;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_KickedPlayer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_WinnedPlayer;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_Vote;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_Microphone;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_Time;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* StartAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* StartTempPhaseAnimation;
	
	UFUNCTION()
	void OnPhaseChanged(EGamePhase NewPhase, int32 RoundNumber, float Duration, ACharacter* MoveForCharacter);
};
