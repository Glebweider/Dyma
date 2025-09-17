// Fill out your copyright notice in the Description page of Project Settings.


#include "FindedSessionWidget.h"

#include "Components/Button.h"
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
		if (Text_SessionName)
		{
			if (auto GameInstance = GetGameInstance<UDF_MainGameInstance>())
			{
				GameInstance->AdvancedJoinSession(Text_SessionName->GetText().ToString(), SessionIndex);
			}
		}
	}
}
