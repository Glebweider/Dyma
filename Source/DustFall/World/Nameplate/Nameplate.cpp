// Fill out your copyright notice in the Description page of Project Settings.


#include "Nameplate.h"
#include "Components/TextRenderComponent.h"

ANameplate::ANameplate()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANameplate::RenderText(const FString& Nickname)
{
	FString ShortNickname = Nickname.Left(10);
	UE_LOG(LogTemp, Warning, TEXT("1111 %s"), *Nickname);
	
	FindComponentByTag<UTextRenderComponent>(TEXT("Forward"))->SetText(FText::FromString(ShortNickname));
	FindComponentByTag<UTextRenderComponent>(TEXT("Backward"))->SetText(FText::FromString(ShortNickname));
}
