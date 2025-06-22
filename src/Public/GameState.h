#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <functional>
#include <vector>
#include "Defines.h"
#include "Interface.h"

class Application;

// Callback types for different game state changes
enum class EGameStateEvent
{
	ScoreChanged,
	HiScoreChanged,
	LevelChanged,
	BallCountChanged,
	TimeChanged,
	GameStarted,
	GameOver,
	LevelUp
};

class FGameState : ITickable
{
public:
	FGameState();
	void ResetValues();
	void ChargeProjectionVelocity();
	void DischargeProjectionVelocity();
	virtual void Tick() override;

	// Game Control
	void StartGame();
	void ResetGame();
	void ScoreBall();

	// Callback system
	void AddCallback(EGameStateEvent eventType, std::function<void()> callback);
	void RemoveCallback(EGameStateEvent eventType, std::function<void()> callback);

	// Getters
	int GetScore() const;
	int GetHiScore() const;
	int GetLevel() const;
	int GetBallCount() const;
	float GetCountDownTime() const;
	float GetSurvivedTime() const;
	float GetAddOnTime() const;
	float GetChargedBallVelocity() const;
	bool GetIsLevelUp() const;
	bool GetIsGameOver() const;
	bool GetIsGameStarted() const;

	// String Getters
	std::string GetLevelString() const;
	std::string GetReqBallString() const;
	std::string GetScoreString() const;
	std::string GetHiScoreString() const;
	std::string GetRemainingTimeString() const;
	std::string GetElapsedTimeMinString() const;
	std::string GetElapsedTimeSecondString() const;

private:
	// Helper Functions
	float GetTimeIncreament(const int currentLevel);
	int GetRequirementBallCount(const int requiredBallCount);
	void NotifyCallbacks(EGameStateEvent eventType);

	// Game Data
	int m_Score;
	int m_HiScore;
	int m_Level;
	int m_RequiredBallCount;
	float m_CountdownTime;
	float m_TotalSurvivedTime;
	float m_AdditionalTime;
	float m_BallVelocity;

	// State Flags
	bool m_IsGameOver;
	bool m_IsLevelUp;
	bool m_IsGameStarted;
	float m_RestartTimer = 3.0f;
	
	// Callback system
	std::vector<std::pair<EGameStateEvent, std::function<void()>>> m_Callbacks;
};

