#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <string>
#include "Defines.h"
#include "Interface.h"

class Application;

class FGameState : ITickable
{
public:
	FGameState();
	void ResetValues();
	void ChargeProjectionVelocity();
	void DischargeProjectionVelocity();
	bool BindApplication(Application* Object);
	virtual void Tick() override;

	void StartGame();
	void ResetGame();
	void ScoreBall() { m_Score++; m_RequiredBallCount--; }

	int GetScore() const { return m_Score; }
	int GetHiScore() const { return m_HiScore; }
	int GetLevel() const { return m_Level; }
	int GetBallCount() const { return m_RequiredBallCount; }

	float GetCountDownTime() const { return m_CountdownTime; }
	float GetSurvivedTime() const { return m_TotalSurvivedTime; }
	float GetAddOnTime() const { return m_AdditionalTime; }
	float GetChargedBallVelocity() const { return m_BallVelocity; }

	bool GetIsLevelUp() const { return m_bIsLevelUp; }
	bool GetIsGameOver() const { return m_bIsGameOver; }
	bool GetIsGameStarted() const { return m_bIsGameStarted; }

	std::string GetLevelString() const { std::ostringstream ss; (ss << m_Level); return ss.str(); }
	std::string GetReqBallString() const { std::ostringstream ss; (ss << m_RequiredBallCount); return ss.str(); }
	std::string GetScoreString() const { std::ostringstream ss; (ss << m_Score); return ss.str(); }
	std::string GetHiScoreString() const { std::ostringstream ss; (ss << m_HiScore); return ss.str(); }
	std::string GetRemainingTimeString() const { std::ostringstream ss; (ss << (int)m_CountdownTime); return ss.str(); }
	std::string GetElapsedTimeMinString() const { std::ostringstream ss; (ss << (int)(m_TotalSurvivedTime / 60.0f)); return ss.str(); }
	std::string GetElapsedTimeSecondString() const { std::ostringstream ss; (ss << ((int)m_TotalSurvivedTime) % 60); return ss.str(); }

private:
	float GetTimeIncreament(const int CurrentLevel);
	int GetRequirementBallCount(const int RequiredBallCount);
	
	int m_Score;
	int m_HiScore;
	int m_Level;
	int m_RequiredBallCount;
	float m_CountdownTime;
	float m_TotalSurvivedTime;
	float m_AdditionalTime;
	float m_BallVelocity;

	bool m_bIsGameOver;
	bool m_bIsLevelUp;
	bool m_bIsGameStarted;
	
	float m_RestartTimer = 3.0f;
	Application* m_ContextObject;
};

