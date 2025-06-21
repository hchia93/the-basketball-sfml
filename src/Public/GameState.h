#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
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
	void ScoreBall()				{ Score++; RequiredBallCount--; }

	int GetScore()					const	{ return Score; }
	int GetHiScore()				const	{ return HiScore; }
	int GetLevel()					const	{ return Level; }
	int GetBallCount()				const	{ return RequiredBallCount; }

	float GetCountDownTime()		const	{ return CountdownTime; }
	float GetSurvivedTime()			const	{ return TotalSurvivedTime; }
	float GetAddOnTime()			const	{ return AdditionalTime; }
	float GetChargedBallVelocity()	const	{ return BallVelocity; }

	bool GetIsLevelUp()				const	{ return bIsLevelUp; }
	bool GetIsGameOver()			const	{ return bIsGameOver; }
	bool GetIsGameStarted()			const	{ return bIsGameStarted; }

	std::string GetLevelString()			const	{ std::ostringstream ss; (ss << Level);  return ss.str(); }
	std::string GetReqBallString()			const	{ std::ostringstream ss; (ss << RequiredBallCount);  return ss.str(); }
	std::string GetScoreString()			const	{ std::ostringstream ss; (ss << Score);  return ss.str(); }
	std::string GetHiScoreString()			const	{ std::ostringstream ss; (ss << HiScore);  return ss.str(); }
	std::string GetRemainingTimeString()	const	{ std::ostringstream ss; (ss << (int)CountdownTime);  return ss.str(); }
	std::string GetElapsedTimeMinString()	const	{ std::ostringstream ss; (ss << (int)(TotalSurvivedTime / 60.0f));  return ss.str(); }
	std::string GetElapsedTimeSecondString()const   { std::ostringstream ss; (ss << ((int)TotalSurvivedTime)%60);  return ss.str(); }

private:

	float GetTimeIncreament(const int CurrentLevel);
	int GetRequirementBallCount(const int RequiredBallCount);
	int Score;
	int HiScore;
	int Level;
	int RequiredBallCount;
	float CountdownTime;
	float TotalSurvivedTime;
	float AdditionalTime;
	float BallVelocity;

	bool bIsGameOver;
	bool bIsLevelUp;
	bool bIsGameStarted;
	
	float RestartTimer = 3.0f;
	Application* ContextObject;
};

