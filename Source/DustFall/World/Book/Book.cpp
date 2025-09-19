// Fill out your copyright notice in the Description page of Project Settings.


#include "Book.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "DustFall/Core/Enums/BookPage.h"


ABook::ABook()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABook::OpenBookAndPage(EBookPage BookPage)
{
	if (!AnimationOpenFirst && !AnimationOpenSecond && !AnimationOpenSecondPage) return;
	
	if (auto SM = FindComponentByClass<USkeletalMeshComponent>())
		switch (BookPage)
		{
			case EBookPage::OpenFirst:
				SM->PlayAnimation(AnimationOpenFirst, false);
				break;
			case EBookPage::OpenSecond:
				SM->PlayAnimation(AnimationOpenSecond, false);
				break;
			case EBookPage::OpenSecondPage:
				SM->PlayAnimation(AnimationOpenSecondPage, false);
				break;
		}
}

void ABook::SetSecondPages(TSubclassOf<UUserWidget> WidgetLeftClass, TSubclassOf<UUserWidget> WidgetRightClass,
	APlayerController* PlayerController)
{
	WidgetLeft = CreateWidget<UUserWidget>(PlayerController, WidgetLeftClass);
	WidgetRight = CreateWidget<UUserWidget>(PlayerController, WidgetRightClass);
	
	auto Widget1 = FindComponentByTag<UWidgetComponent>("Page2");
	auto Widget2 = FindComponentByTag<UWidgetComponent>("Page3");
	if (Widget1 && Widget2)
	{
		Widget1->SetWidget(WidgetLeft);
		Widget2->SetWidget(WidgetRight);
	}
}
