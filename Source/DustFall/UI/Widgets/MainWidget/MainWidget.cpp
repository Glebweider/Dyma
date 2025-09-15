// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "DustFall/Core/GameInstance/DF_MainGameInstance.h"
#include "DustFall/UI/Widgets/FindedSessionWidget/FindedSessionWidget.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainGameInstance = GetGameInstance<UDF_MainGameInstance>();
	
	if (Btn_ApplyCreateSession)
	{
		Btn_ApplyCreateSession->OnClicked.AddDynamic(this, &UMainWidget::OnApplyCreateSessionClicked);
	}

	if (MainGameInstance)
	{
		MainGameInstance->InitUniquePlayerId();
	}
}

void UMainWidget::OnApplyCreateSessionClicked()
{
	if (!EditText_CreateSessionName) return;
	
	FText SessionName = EditText_CreateSessionName->GetText();
	FString SessionNameStr = SessionName.ToString().TrimStartAndEnd();
	
	if (!SessionName.IsEmpty() && SessionNameStr.Len() >= 4 && SessionNameStr.Len() <= 16)
	{
		if (MainGameInstance)
		{
			MainGameInstance->AdvancedCreateSession(SessionNameStr);
		}
	}
	else
	{
		// Вывести ошибку, если имя сессии не соответствует требованиям
		UE_LOG(LogTemp, Warning, TEXT("Имя сессии должно быть от 4 до 16 символов!"));
	}
}

void UMainWidget::StartFindSessions()
{
	if (WSwitch_FindSession && EditText_FindSessionName && MainGameInstance)
	{
		FText SessionName = EditText_CreateSessionName->GetText();
		FString SessionNameStr = SessionName.ToString().TrimStartAndEnd();
		
		WSwitch_FindSession->SetActiveWidgetIndex(1);

		FindProxySession = MainGameInstance->AdvancedFindSessions(SessionNameStr);
		if (FindProxySession)
		{
			if (!FindProxySession) return;

			FindProxySession->OnSuccess.AddDynamic(this, &UMainWidget::OnFindSessionSuccess);
			FindProxySession->OnFailure.AddDynamic(this, &UMainWidget::OnFindSessionFailure);
			FindProxySession->Activate();
		}
	}
}

void UMainWidget::OnFindSessionSuccess(const TArray<FBlueprintSessionResult>& Results)
{
	FString Message = FString::Printf(TEXT("Открытие карты. Найдено сессий: %d"), Results.Num());

	GEngine->AddOnScreenDebugMessage(
		-1,
		15.0f,
		FColor::Green,
		Message
	);
	
	if (WSwitch_FindSession && FindedSessionWidgetClass && VBox_FindedSessionsList)
	{
		if (Results.Num() > 0)
		{
			VBox_FindedSessionsList->ClearChildren();

			MainGameInstance->OnlineSessionResults.Empty();
			
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				const auto& Session = Results[i];
				auto Widget = CreateWidget<UFindedSessionWidget>(this, FindedSessionWidgetClass);
				auto SessionResult = Session.OnlineResult;

				MainGameInstance->OnlineSessionResults.Add(SessionResult);

				if (Widget && SessionResult.IsValid())
				{
					Widget->SessionIndex = i;
					
					FString SessionName;
					SessionResult.Session.SessionSettings.Get(FName("SESSION_NAME"), SessionName);
					Widget->Text_SessionName->SetText(FText::FromString(SessionName));
					
					Widget->Text_PlayerCounter->SetText(
						FText::FromString(
							FString::Printf(TEXT("%d/%d"),
								SessionResult.Session.SessionSettings.NumPublicConnections -
								SessionResult.Session.NumOpenPublicConnections,
								SessionResult.Session.SessionSettings.NumPublicConnections)
						)
					);
					
					Widget->Text_Ping->SetText(FText::AsNumber(SessionResult.PingInMs));

					VBox_FindedSessionsList->AddChild(Widget);
				}
			}
			
			WSwitch_FindSession->SetActiveWidgetIndex(0);
		} else
		{
			WSwitch_FindSession->SetActiveWidgetIndex(2);
		}
	}
}

void UMainWidget::OnFindSessionFailure(const TArray<FBlueprintSessionResult>& Results)
{
	if (WSwitch_FindSession)
	{
		WSwitch_FindSession->SetActiveWidgetIndex(2);
	}
}
