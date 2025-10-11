// Fill out your copyright notice in the Description page of Project Settings.


#include "DF_HUD.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Dyma/Characters/Player/Interfaces/PlayerStateInterface.h"
#include "Dyma/Core/GameState/DF_GameState.h"
#include "Dyma/UI/Widgets/FindedSession/FindedSessionWidget.h"
#include "GameFramework/PlayerState.h"

void UDF_HUD::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ADF_GameState* GS = Cast<ADF_GameState>(GetWorld()->GetGameState()))
		GS->OnPhaseChanged.AddDynamic(this, &UDF_HUD::OnPhaseChanged);
}

void UDF_HUD::UpdateMicrophoneState_Implementation(bool bIsActive)
{
	if (!Img_Microphone) return;

	Img_Microphone->SetRenderOpacity(bIsActive ? 1.0f : 0.7f);
}

void UDF_HUD::SetHelpVoteTextVisible_Implementation()
{
	if (!Text_HelpVote) return;

	Text_HelpVote->SetVisibility(ESlateVisibility::Collapsed);
}

void UDF_HUD::SetVoteText_Implementation(const FString& NewText)
{
	if (!Text_Vote) return;
	
	Text_Vote->SetText(FText::Format(NSLOCTEXT("HUD", "VoteText", "ГОЛОС ПРОТИВ: {0}"), FText::FromString(NewText)));
	Text_Vote->SetVisibility(ESlateVisibility::Visible);
}

void UDF_HUD::SetCastVote_Implementation(bool bIsVoteBtnPressed)
{
	if (!ProgressBar_Vote) return;
	
	if (!bIsVoteBtnPressed)
	{
		GetWorld()->GetTimerManager().ClearTimer(VoteProgressTimer);
		VoteProgressElapsed = 0.f;

		ProgressBar_Vote->SetVisibility(ESlateVisibility::Hidden);
		ProgressBar_Vote->SetPercent(0.f);
		return;
	}
	
	ProgressBar_Vote->SetVisibility(ESlateVisibility::Visible);
	ProgressBar_Vote->SetPercent(0.f);
	VoteProgressElapsed = 0.f;
	
	GetWorld()->GetTimerManager().SetTimer(
		VoteProgressTimer,
		this,
		&UDF_HUD::UpdateVoteProgress,
		0.02f,
		true
	);
}

void UDF_HUD::SetKickedPlayerName_Implementation(const FString& PlayerName)
{
	if (Text_KickedPlayer)
		Text_KickedPlayer->SetText(FText::Format(NSLOCTEXT("HUD", "KickedPlayer", "Бюрократ {0} уволен"), FText::FromString(PlayerName)));
}

void UDF_HUD::UpdateVoteProgress()
{
	if (!ProgressBar_Vote) return;

	VoteProgressElapsed += 0.02f;
	float Alpha = FMath::Clamp(VoteProgressElapsed / 2.f, 0.f, 1.f);

	ProgressBar_Vote->SetPercent(Alpha);

	if (Alpha >= 1.f)
		GetWorld()->GetTimerManager().ClearTimer(VoteProgressTimer);
}

void UDF_HUD::UpdateCountdown()
{
	if (!Text_Time) return;
	
	int32 Minutes = RemainingSeconds / 60;
	int32 Seconds = RemainingSeconds % 60;

	FString TimeStr = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	Text_Time->SetText(FText::FromString(TimeStr));

	if (RemainingSeconds <= 0)
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
	else
		--RemainingSeconds;
}

void UDF_HUD::OnPhaseChanged(EGamePhase NewPhase, int32 RoundNumber, float Duration, ACharacter* MoveForCharacter)
{
	if (!Text_Phase && !Text_MoveFor && !Text_Time && !Text_Vote && !Text_HelpVote && !ProgressBar_Vote && !Text_KickedPlayer) return;

	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
	
	Text_MoveFor->SetVisibility(ESlateVisibility::Visible);
	Text_Time->SetVisibility(ESlateVisibility::Visible);
	Text_Vote->SetVisibility(ESlateVisibility::Collapsed);
	Text_HelpVote->SetVisibility(ESlateVisibility::Collapsed);
	ProgressBar_Vote->SetVisibility(ESlateVisibility::Collapsed);
	Text_KickedPlayer->SetVisibility(ESlateVisibility::Collapsed);
	
	FText PhaseName;
	switch (NewPhase)
	{
		case EGamePhase::Lobby:
			PhaseName = FText::GetEmpty();
			break;
		case EGamePhase::DocReview:
			{
				PhaseName = NSLOCTEXT("HUD", "DocReview", "0-Й КРУГ: ОЗНАКОМЛЕНИЕ");
				Text_MoveFor->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;
		case EGamePhase::RoundIntro:
			PhaseName = NSLOCTEXT("HUD", "RoundIntro", "0-Й КРУГ: ЗНАКОМСТВА");
			break;
		case EGamePhase::Round:
			PhaseName = FText::Format(NSLOCTEXT("HUD", "Round", "{0}-Й КРУГ: РАСКРЫТИЕ"), RoundNumber);
			break;
		case EGamePhase::Debate:
			{
				PhaseName = FText::Format(NSLOCTEXT("HUD", "Debate", "{0}-Й КРУГ: ОБСУЖДЕНИЕ"), RoundNumber);
				Text_MoveFor->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;
		case EGamePhase::Vote:
			{
				PhaseName = FText::Format(NSLOCTEXT("HUD", "Vote", "{0}-Й КРУГ: ГОЛОСОВАНИЕ"), RoundNumber);
				Text_Vote->SetText(NSLOCTEXT("HUD", "VoteText", "ГОЛОС ПРОТИВ:"));
				
				Text_MoveFor->SetVisibility(ESlateVisibility::Collapsed);
				
				if (IPlayerStateInterface::Execute_GetIsParticipant(GetOwningPlayerPawn()->GetPlayerState()))
				{
					Text_Vote->SetVisibility(ESlateVisibility::Visible);
					Text_HelpVote->SetVisibility(ESlateVisibility::Visible);					
				}
			}
			break;
		case EGamePhase::Elimination:
			{
				PhaseName = FText::Format(NSLOCTEXT("HUD", "Vote", "{0}-Й КРУГ: ИТОГИ"), RoundNumber);
				
				if (IsValid(MoveForCharacter))
				{
					if (auto PS = MoveForCharacter->GetPlayerState())
						Text_KickedPlayer->SetText(FText::Format(NSLOCTEXT("HUD", "KickedPlayer", "Бюрократ {0} уволен"), FText::FromString(PS->GetPlayerName().Left(10))));
				} else {
					Text_KickedPlayer->SetText(NSLOCTEXT("HUD", "KickedPlayer", "Бюрократ не найден"));
				}
						
				Text_KickedPlayer->SetVisibility(ESlateVisibility::Visible);
				Text_MoveFor->SetVisibility(ESlateVisibility::Collapsed);
				Text_Time->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;
		case EGamePhase::FinalVote:
			{
				PhaseName = NSLOCTEXT("HUD", "Vote", "Финальное ГОЛОСОВАНИЕ");
				Text_Vote->SetText(NSLOCTEXT("HUD", "VoteText", "ГОЛОС ПРОТИВ:"));
					
				Text_MoveFor->SetVisibility(ESlateVisibility::Collapsed);
					
				if (!IPlayerStateInterface::Execute_GetIsParticipant(GetOwningPlayerPawn()->GetPlayerState()))
				{
					Text_Vote->SetVisibility(ESlateVisibility::Visible);
					Text_HelpVote->SetVisibility(ESlateVisibility::Visible);					
				}
			}
			break;
		default:
			PhaseName = NSLOCTEXT("HUD", "Unknown", "Unknown");
			break;
	}
		
	Text_Phase->SetText(PhaseName);

	if (MoveForCharacter)
	{
		if (auto PS = MoveForCharacter->GetPlayerState())
			Text_MoveFor->SetText(FText::Format(NSLOCTEXT("HUD", "VoteFor", "{0}"), FText::FromString(PS->GetPlayerName().Left(10))));
	} else {
		Text_MoveFor->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Duration > 0.f && Text_Time->GetVisibility() == ESlateVisibility::Visible)
	{
		RemainingSeconds = FMath::RoundToInt(Duration);
		UpdateCountdown();

		GetWorld()->GetTimerManager().SetTimer(
			CountdownTimerHandle,
			this,
			&UDF_HUD::UpdateCountdown,
			1.0f,
			true
		);
	}
}
