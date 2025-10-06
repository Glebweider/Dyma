// Fill out your copyright notice in the Description page of Project Settings.


#include "BookCustomizationWidget.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Dyma/Core/GameInstance/DF_MainGameInstance.h"
#include "Dyma/Core/Structures/Face.h"
#include "Dyma/UI/Widgets/UserFaceSlot/UserFaceSlotWidget.h"

void UBookCustomizationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto PC = GetOwningPlayer();
	if (!UniformGridPanel && !TableFaces && !UserFaceSlotWidget && !PC) return;

	auto GameInstance = Cast<UDF_MainGameInstance>(GetGameInstance());
	if (!GameInstance) return;

	UniformGridPanel->ClearChildren();
	
	const TArray<FName> OutRowNames = TableFaces->GetRowNames();
	for (int i = 0; i < OutRowNames.Num(); i++)
	{
		FName RowName = OutRowNames[i];
		
		static const FString ContextString(TEXT("Getting FaceData Row"));
		FFaceData* RowTable = TableFaces->FindRow<FFaceData>(RowName, ContextString);
		if (RowTable)
			if (auto Widget = CreateWidget<UUserFaceSlotWidget>(PC, UserFaceSlotWidget))
			{
				GameInstance->OnFaceRowNameChanged.AddDynamic(Widget, &UUserFaceSlotWidget::HandleFaceRowChanged);
				
				int32 Row = i / Columns;
				int32 Col = i % Columns;

				Widget->SetRowName(RowName);
				Widget->SetImage(RowTable->FaceClose);
				
				if (RowName == GameInstance->FaceRowName)
					Widget->HandleFaceRowChanged(RowName);

				UniformGridPanel->AddChildToUniformGrid(Widget, Row, Col);
			}
	}
}
