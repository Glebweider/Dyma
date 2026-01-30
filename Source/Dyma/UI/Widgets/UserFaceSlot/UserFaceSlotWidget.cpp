// Fill out your copyright notice in the Description page of Project Settings.


#include "UserFaceSlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Dyma/Core/Interface/GameInstanceInterface.h"

void UUserFaceSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn_Slot) {
		Btn_Slot->OnClicked.AddDynamic(this, &UUserFaceSlotWidget::OnSelectFace);
	}
}

void UUserFaceSlotWidget::OnSelectFace()
{
	if (auto GameInstance = GetGameInstance())
		IGameInstanceInterface::Execute_SetPlayerFace(GameInstance, RowName);
}

void UUserFaceSlotWidget::HandleFaceRowChanged(FName NewFaceRowName)
{
	FSlateBrush Brush = Image->GetBrush();
	
	Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
	Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;

	if (NewFaceRowName == RowName)
	{
		FSlateColor Color = FSlateColor(FLinearColor(0.51f, 0.00f, 0.11f, 1.00f));
		
		Brush.OutlineSettings.Width = 5.f;
		Brush.OutlineSettings.Color = Color;
		
		Text->SetColorAndOpacity(Color);
	} else {
		Brush.OutlineSettings.Width = 0.f;

		Text->SetColorAndOpacity(FLinearColor(0.f, 0.0f, 0.f, 1.00f));
	}
	
	Image->SetBrush(Brush);
}

void UUserFaceSlotWidget::SetRowName(const FName& InRowName)
{
	RowName = InRowName;

	if (Text)
		Text->SetText(FText::FromName(InRowName));
}

void UUserFaceSlotWidget::SetImage(UTexture2D* Texture)
{
	if (!Image || !Texture) return;

	FSlateBrush Brush;
	Brush.SetResourceObject(Texture);
	Brush.ImageSize.X = Texture->GetSizeX();
	Brush.ImageSize.Y = Texture->GetSizeY();

	Image->SetBrush(Brush);
}
