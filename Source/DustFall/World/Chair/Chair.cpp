// Fill out your copyright notice in the Description page of Project Settings.


#include "Chair.h"
#include "Blueprint/UserWidget.h"
#include "DustFall/World/Book/Book.h"
#include "DustFall/World/Nameplate/Nameplate.h"
#include "Net/UnrealNetwork.h"


AChair::AChair()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AChair::Multi_StartGame_Implementation()
{
	if (Book && WidgetLeftClass && WidgetRightClass) {
		if (auto PC = Cast<APlayerController>(GetOwner()))
			if (PC->IsLocalController())
				Book->SetSecondPages(WidgetLeftClass, WidgetRightClass, PC);
		
		Book->Server_OpenBookAndPage(EBookPage::OpenSecondPage);
	}
}

void AChair::StartGame_Implementation()
{
	Multi_StartGame();
}

void AChair::Destroyed()
{
	Super::Destroyed();

	if (Nameplate)
		Nameplate->Destroy();

	if (Book)
		Book->Destroy();
}

void AChair::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChair, Character);
	DOREPLIFETIME(AChair, Nameplate);
	DOREPLIFETIME(AChair, Book);
}