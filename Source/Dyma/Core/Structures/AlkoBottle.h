#pragma once

#include "CoreMinimal.h"
#include "AlkoBottle.generated.h"

USTRUCT(BlueprintType)
struct DYMA_API FAlkoBottleData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Duration;
};
