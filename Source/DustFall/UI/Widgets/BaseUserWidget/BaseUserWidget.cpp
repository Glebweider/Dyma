// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUserWidget.h"

void UBaseUserWidget::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
