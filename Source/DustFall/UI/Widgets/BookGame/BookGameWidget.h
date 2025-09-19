// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DustFall/Core/Structures/Project.h"
#include "BookGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUSTFALL_API UBookGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite)
	FProjectData Project;
};
