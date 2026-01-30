#pragma once


UENUM(BlueprintType)
enum class EBookPage : uint8
{
	OpenFirst      UMETA(DisplayName = "Open First Page"),
	OpenSecond     UMETA(DisplayName = "Open Second Page"),
	OpenSecondPage UMETA(DisplayName = "Open Second Page")
};