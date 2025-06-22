#pragma once
#include <sstream>
#include "Defines.h"
#include "Interface.h"

namespace sf {
	class Music;
	class Font;
	class Texture;
}
class Application;

class FGameState : ITickable
{
public:
	FGameState();

	void ResetValues();
	void ChargeProjectionVelocity();
	void DischargeProjectionVelocity();
	bool BindApplication(Application* object);
	virtual void Tick() override;

	void StartGame();
	void ResetGame();
	void ScoreBall();

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

	std::string GetLevelString() const;
	std::string GetReqBallString() const;
	std::string GetScoreString() const;
	std::string GetHiScoreString() const;
	std::string GetRemainingTimeString() const;
	std::string GetElapsedTimeMinString() const;
	std::string GetElapsedTimeSecondString() const;

private:
	float GetTimeIncreament(const int currentLevel);
	int GetRequirementBallCount(const int requiredBallCount);

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

