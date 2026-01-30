#pragma once


UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Lobby,              // Игроки в лобби
	DocReview,          // 3 мин – ознакомление с документами
	Round,              // 2-й круг
	Debate,             // Круг споров (45 сек) – повторяется пока >2 игроков
	Vote,               // Голосованние за исключенние игрока
	Elimination,        // кик игрока и подведение итогов
	FinalVote,          // Голосование зрителей
	Finished,           // Конец игры
	NewLobby            // Лобби новой игры 
};