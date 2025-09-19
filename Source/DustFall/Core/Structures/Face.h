#pragma once

#include "CoreMinimal.h"
#include "Face.generated.h"

USTRUCT(BlueprintType)
struct DUSTFALL_API FFaceData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* FaceOpen;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* FaceClose;
};
