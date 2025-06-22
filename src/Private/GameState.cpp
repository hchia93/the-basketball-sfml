#include "GameState.h"
#include "Application.h"
#include <sstream> // Required for Get...String() implementations

FGameState::FGameState()
{
	ResetValues();
}

void FGameState::ResetValues()
{
	m_Score = 0;
	m_HiScore = 0;
	m_Level = 1;
	m_RequiredBallCount = GetRequirementBallCount(m_Level);
	m_AdditionalTime = GetTimeIncreament(m_RequiredBallCount);
	m_CountdownTime = m_AdditionalTime;
	m_TotalSurvivedTime = 0.0f;

	m_bIsGameStarted = false;
	m_bIsGameOver = false;
	m_bIsLevelUp = false;
	m_BallVelocity = 5.0f;
	m_RestartTimer = 3.0f;
}

void FGameState::ChargeProjectionVelocity()
{
	if (m_BallVelocity <= 60.0f)
	{
		m_BallVelocity += (DELTA_TIME_STEP * 5.0f);
	}
	else
	{
		m_BallVelocity = 60.0f;
	}
}

void FGameState::DischargeProjectionVelocity()
{
	if (m_BallVelocity >= 5.0f)
	{
		m_BallVelocity -= (DELTA_TIME_STEP * 5.0f);
	}
	else
	{
		m_BallVelocity = 5.0f;
	}
}

bool FGameState::BindApplication(Application* object)
{
	m_ContextObject = object;
	return m_ContextObject != nullptr;
}

void FGameState::Tick()
{
	if (m_bIsGameStarted)
	{
		// Update scores
		if (m_Score >= m_HiScore)
		{
			m_HiScore = m_Score;
		}

		// Check Level up
		if (m_RequiredBallCount <= 0)
		{
			m_Level += 1;
			m_RequiredBallCount = GetRequirementBallCount(m_Level);
			m_AdditionalTime = GetTimeIncreament(m_RequiredBallCount);
			m_CountdownTime += m_AdditionalTime;
			m_bIsLevelUp = true;
		}
		else
		{
			m_bIsLevelUp = false;
		}

		// Check GameOver
		m_bIsGameOver = (m_CountdownTime <= 0);
		if (m_bIsGameOver)
		{
			m_RestartTimer -= DELTA_TIME_STEP;
		}

		if (m_RestartTimer < 0.0f)
		{
			ResetValues();
		}

		m_CountdownTime -= DELTA_TIME_STEP;
		m_TotalSurvivedTime += DELTA_TIME_STEP;
	}
}

void FGameState::StartGame()
{
	if (!m_bIsGameStarted)
	{
		m_bIsGameStarted = true;
	}
}

void FGameState::ResetGame()
{
	m_bIsGameStarted = false;
	ResetValues();
}

float FGameState::GetTimeIncreament(const int ballCount)
{
	return (ballCount * 8.0f);
}

int FGameState::GetRequirementBallCount(const int currentLevel)
{
	return (currentLevel * 5);
}

void FGameState::ScoreBall()
{
	m_Score++;
	m_RequiredBallCount--;
}

int FGameState::GetScore() const { return m_Score; }
int FGameState::GetHiScore() const { return m_HiScore; }
int FGameState::GetLevel() const { return m_Level; }
int FGameState::GetBallCount() const { return m_RequiredBallCount; }
float FGameState::GetCountDownTime() const { return m_CountdownTime; }
float FGameState::GetSurvivedTime() const { return m_TotalSurvivedTime; }
float FGameState::GetAddOnTime() const { return m_AdditionalTime; }
float FGameState::GetChargedBallVelocity() const { return m_BallVelocity; }
bool FGameState::GetIsLevelUp() const { return m_bIsLevelUp; }
bool FGameState::GetIsGameOver() const { return m_bIsGameOver; }
bool FGameState::GetIsGameStarted() const { return m_bIsGameStarted; }

std::string FGameState::GetLevelString() const
{
	std::ostringstream ss;
	ss << m_Level;
	return ss.str();
}

std::string FGameState::GetReqBallString() const
{
	std::ostringstream ss;
	ss << m_RequiredBallCount;
	return ss.str();
}

std::string FGameState::GetScoreString() const
{
	std::ostringstream ss;
	ss << m_Score;
	return ss.str();
}

std::string FGameState::GetHiScoreString() const
{
	std::ostringstream ss;
	ss << m_HiScore;
	return ss.str();
}

std::string FGameState::GetRemainingTimeString() const
{
	std::ostringstream ss;
	ss << (int)m_CountdownTime;
	return ss.str();
}

std::string FGameState::GetElapsedTimeMinString() const
{
	std::ostringstream ss;
	ss << (int)(m_TotalSurvivedTime / 60.0f);
	return ss.str();
}

std::string FGameState::GetElapsedTimeSecondString() const
{
	std::ostringstream ss;
	ss << ((int)m_TotalSurvivedTime) % 60;
	return ss.str();
}
