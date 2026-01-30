#pragma once

#include "CoreMinimal.h"
#include "Project.generated.h"

USTRUCT(BlueprintType)
struct DYMA_API FProjectData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Status;

	UPROPERTY(BlueprintReadWrite)
	FString Goal;

	UPROPERTY(BlueprintReadWrite)
	FString Consequence;

	UPROPERTY(BlueprintReadWrite)
	FString Implementation;

	UPROPERTY(BlueprintReadWrite)
	FString Financing;

	UPROPERTY(BlueprintReadWrite)
	FString Support;
};
