// Fill out your copyright notice in the Description page of Project Settings.


#include "BookGameWidget.h"
#include "DustFall/Characters/Player/Interfaces/PlayerStateInterface.h"
#include "GameFramework/PlayerState.h"

void UBookGameWidget::NativePreConstruct()
{
	Super::NativeConstruct();

	if (GetOwningPlayer() && GetOwningPlayer()->PlayerState)
		Project = IPlayerStateInterface::Execute_GetProjectData(GetOwningPlayer()->PlayerState);
}
