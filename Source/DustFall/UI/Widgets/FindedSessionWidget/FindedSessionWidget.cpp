// Fill out your copyright notice in the Description page of Project Settings.


#include "FindedSessionWidget.h"

#include "Components/Button.h"
#include "Engine/Engine.h"
#include "Components/TextBlock.h"
#include "DustFall/Core/GameInstance/DF_MainGameInstance.h"

void UFindedSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_JoinSession)
	{
		Btn_JoinSession->OnClicked.AddDynamic(this, &UFindedSessionWidget::OnJoinSessionClicked);
	}
}

void UFindedSessionWidget::OnJoinSessionClicked()
{
	const double CurrentTime = FPlatformTime::Seconds();
	const double TimeSinceLastClick = CurrentTime - LastClickTime;
	LastClickTime = CurrentTime;

	if (TimeSinceLastClick <= DoubleClickThreshold)
	{
		
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Emerald, TEXT("OnConnect"));
	if (Text_SessionName)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Emerald, TEXT("Text_SessionName success"));
		if (auto GameInstance = GetGameInstance<UDF_MainGameInstance>())
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Emerald, TEXT("GameInstance success"));
			GameInstance->AdvancedJoinSession(Text_SessionName->GetText().ToString(), SessionIndex);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Emerald, TEXT("GameInstance error"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Emerald, TEXT("Text_SessionName error"));
	}
}
