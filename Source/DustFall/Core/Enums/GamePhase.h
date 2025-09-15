#pragma once


UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Lobby,              // Игроки в лобби
	DocReview,          // 3 мин – ознакомление с документами
	RoundIntro,         // 1-й круг: знакомство (30 сек)
	Round,              // 2-й круг
	Debate,             // Круг споров (45 сек) – повторяется пока >2 игроков
	RoundBreak,         // 3 мин – добровольный перерыв
	PreFinalPause,      // 30 сек – перерыв перед финалом
	FinalVote,          // Голосование зрителей
	Finished            // Конец игры
};