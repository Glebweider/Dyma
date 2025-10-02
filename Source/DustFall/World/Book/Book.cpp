// Fill out your copyright notice in the Description page of Project Settings.


#include "Book.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "DustFall/Core/Enums/BookPage.h"
#include "DustFall/Core/Enums/Pages.h"


ABook::ABook()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABook::Server_OpenBookAndPage_Implementation(EBookPage BookPage)
{
	Multi_OpenBookAndPage(BookPage);
}

void ABook::Multi_OpenBookAndPage_Implementation(EBookPage BookPage)
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

void ABook::ShowPages(EPages PageType)
{
	for (TPair Page : Pages)
	{
		for (UWidgetComponent* WidgetComponent : Page.Value)
		{
			if (!WidgetComponent) continue;

			WidgetComponent->SetVisibility(Page.Key == PageType);
			WidgetComponent->SetCollisionProfileName(Page.Key == PageType ? FName("UI") : FName("NoCollision"));
		}
	}
}

void ABook::AddPages(EPages PageType, TArray<UWidgetComponent*> WidgetComponents)
{
	if (Pages.Contains(PageType))
	{
		for (auto Page : Pages)
		{
			if (Page.Key == PageType)
			{
				for (auto Component : WidgetComponents)
				{
					Page.Value.Add(Component);
				}
				
				break;
			}
		}
	}
	else
	{
		Pages.Add(PageType, WidgetComponents);
	}
}
