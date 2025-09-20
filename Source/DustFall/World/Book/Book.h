// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DustFall/Core/Enums/BookPage.h"
#include "GameFramework/Actor.h"
#include "Book.generated.h"

UCLASS()
class DUSTFALL_API ABook : public AActor
{
	GENERATED_BODY()

public:
	ABook();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_OpenBookAndPage(EBookPage BookPage);

	UFUNCTION(BlueprintCallable)
	void SetSecondPages(TSubclassOf<UUserWidget> WidgetLeftClass, TSubclassOf<UUserWidget> WidgetRightClass, APlayerController* PlayerController);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OpenBookAndPage(EBookPage BookPage);
	
	UPROPERTY()
	UUserWidget* WidgetLeft;

	UPROPERTY()
	UUserWidget* WidgetRight;

	UPROPERTY(EditAnywhere, Category="Animation")
	UAnimationAsset* AnimationOpenFirst;

	UPROPERTY(EditAnywhere, Category="Animation")
	UAnimationAsset* AnimationOpenSecond;

	UPROPERTY(EditAnywhere, Category="Animation")
	UAnimationAsset* AnimationOpenSecondPage;
};
