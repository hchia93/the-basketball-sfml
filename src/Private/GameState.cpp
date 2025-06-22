#include "GameState.h"
#include "Application.h"

FGameState::FGameState()
{
	ResetValues();
}

void FGameState::ResetValues()
{
	Score = 0;
	HiScore = 0;
	Level = 1;
	RequiredBallCount = GetRequirementBallCount(Level); 
	AdditionalTime = GetTimeIncreament(RequiredBallCount); 
	CountdownTime = AdditionalTime;
	TotalSurvivedTime = 0.0f;

	bIsGameStarted = false;
	bIsGameOver = false;
	bIsLevelUp = false;
	BallVelocity = 5.0f;
	RestartTimer = 3.0f;
}

void FGameState::ChargeProjectionVelocity()
{
	BallVelocity = (BallVelocity <= 60.0f) ? BallVelocity + (DELTA_TIME_STEP * 5.0f) : 60.0f;
}

void FGameState::DischargeProjectionVelocity()
{
	BallVelocity = (BallVelocity >= 5.0f) ? BallVelocity - (DELTA_TIME_STEP * 5.0f) : 5.0f;
}

bool FGameState::BindApplication(Application * Object)
{
	ContextObject = Object;
	return ContextObject != nullptr;
}

void FGameState::Tick()
{
	if (bIsGameStarted)
	{
		// Update scores
		if (Score >= HiScore)
			HiScore = Score;

		// Check Level up
		if (RequiredBallCount <= 0)
		{
			Level += 1;
			RequiredBallCount = GetRequirementBallCount(Level);
			AdditionalTime = GetTimeIncreament(RequiredBallCount);
			CountdownTime += AdditionalTime;
			bIsLevelUp = true;
		}
		else
		{
			bIsLevelUp = false;
		}

		// Check GameOver
		bIsGameOver = (CountdownTime <= 0);
		if (bIsGameOver)
			RestartTimer -= DELTA_TIME_STEP;

		if (RestartTimer < 0.0f)
			ResetValues();

		CountdownTime -= DELTA_TIME_STEP;
		TotalSurvivedTime += DELTA_TIME_STEP;
	}
}

void FGameState::StartGame()
{
	if (!bIsGameStarted)
		bIsGameStarted = true;
}

void FGameState::ResetGame()
{
	bIsGameStarted = false;
	ResetValues();
}

float FGameState::GetTimeIncreament(const int BallCount)
{
	return (BallCount * 8.0f);
}

int FGameState::GetRequirementBallCount(const int CurrentLevel)
{
	return (CurrentLevel * 5);
}
