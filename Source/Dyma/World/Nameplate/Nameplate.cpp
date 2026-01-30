// Fill out your copyright notice in the Description page of Project Settings.


#include "Nameplate.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

ANameplate::ANameplate()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANameplate::OnRep_Username()
{
	RenderText(Username);
}

void ANameplate::RenderText(const FString& Nickname)
{
	FString ShortNickname = Nickname.Left(10);

	FindComponentByTag<UTextRenderComponent>(TEXT("Forward"))->SetText(FText::FromString(ShortNickname));
	FindComponentByTag<UTextRenderComponent>(TEXT("Backward"))->SetText(FText::FromString(ShortNickname));
}

void ANameplate::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANameplate, Username);
}
