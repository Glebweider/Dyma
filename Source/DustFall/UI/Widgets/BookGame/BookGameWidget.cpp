// Fill out your copyright notice in the Description page of Project Settings.


#include "BookGameWidget.h"
#include "DustFall/Core/Interface/GameStateInterface.h"
#include "GameFramework/PlayerState.h"

void UBookGameWidget::NativePreConstruct()
{
	Super::NativeConstruct();

	if (GetOwningPlayer() && GetOwningPlayer()->PlayerState)
		Project = IGameStateInterface::Execute_GetProjectData(GetOwningPlayer()->PlayerState);
}
