// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BookCustomizationWidget.generated.h"

class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class DYMA_API UBookCustomizationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pagination")
	int32 PageIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pagination")
	int32 ItemsPerPage = 12;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	UDataTable* TableFaces;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> UserFaceSlotWidget;
	
	/** Binds */
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGridPanel;
	
	int32 Columns = 4;
};
