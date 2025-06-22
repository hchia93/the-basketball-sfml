#include "GameState.h"
#include "Application.h"

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
	m_IsGameStarted = false;
	m_IsGameOver = false;
	m_IsLevelUp = false;
	m_BallVelocity = 5.0f;
	m_RestartTimer = 3.0f;
}

void FGameState::ChargeProjectionVelocity()
{
	m_BallVelocity = (m_BallVelocity <= 60.0f) ? m_BallVelocity + (DELTA_TIME_STEP * 5.0f) : 60.0f;
}

void FGameState::DischargeProjectionVelocity()
{
	m_BallVelocity = (m_BallVelocity >= 5.0f) ? m_BallVelocity - (DELTA_TIME_STEP * 5.0f) : 5.0f;
}

void FGameState::Tick()
{
	if (m_IsGameStarted)
	{
		// Update scores
		if (m_Score >= m_HiScore)
		{
			m_HiScore = m_Score;
			NotifyCallbacks(EGameStateEvent::HiScoreChanged);
		}

		// Check Level up
		if (m_RequiredBallCount <= 0)
		{
			m_Level += 1;
			m_RequiredBallCount = GetRequirementBallCount(m_Level);
			m_AdditionalTime = GetTimeIncreament(m_RequiredBallCount);
			m_CountdownTime += m_AdditionalTime;
			m_IsLevelUp = true;
			NotifyCallbacks(EGameStateEvent::LevelChanged);
			NotifyCallbacks(EGameStateEvent::BallCountChanged);
			NotifyCallbacks(EGameStateEvent::LevelUp);
		}
		else
		{
			m_IsLevelUp = false;
		}

		// Check GameOver
		m_IsGameOver = (m_CountdownTime <= 0);
		if (m_IsGameOver)
		{
			m_RestartTimer -= DELTA_TIME_STEP;
			NotifyCallbacks(EGameStateEvent::GameOver);
		}

		if (m_RestartTimer < 0.0f)
		{
			ResetValues();
		}

		m_CountdownTime -= DELTA_TIME_STEP;
		m_TotalSurvivedTime += DELTA_TIME_STEP;
		
		// Notify time changes every frame when game is running
		NotifyCallbacks(EGameStateEvent::TimeChanged);
	}
}

void FGameState::StartGame()
{
	if (!m_IsGameStarted)
	{
		m_IsGameStarted = true;
		NotifyCallbacks(EGameStateEvent::GameStarted);
	}
}

void FGameState::ResetGame()
{
	m_IsGameStarted = false;
	ResetValues();
	// Notify all widgets to update
	NotifyCallbacks(EGameStateEvent::ScoreChanged);
	NotifyCallbacks(EGameStateEvent::HiScoreChanged);
	NotifyCallbacks(EGameStateEvent::LevelChanged);
	NotifyCallbacks(EGameStateEvent::BallCountChanged);
	NotifyCallbacks(EGameStateEvent::TimeChanged);
}

void FGameState::ScoreBall()
{
	m_Score++;
	m_RequiredBallCount--;
	NotifyCallbacks(EGameStateEvent::ScoreChanged);
	NotifyCallbacks(EGameStateEvent::BallCountChanged);
}

// Callback system
void FGameState::AddCallback(EGameStateEvent eventType, std::function<void()> callback)
{
	m_Callbacks.emplace_back(eventType, callback);
}

void FGameState::RemoveCallback(EGameStateEvent eventType, std::function<void()> callback)
{
	// Note: This is a simple implementation. In a more complex system, you might want to use a more sophisticated approach
	m_Callbacks.erase(
		std::remove_if(m_Callbacks.begin(), m_Callbacks.end(),
			[eventType, &callback](const auto& pair) {
				return pair.first == eventType && pair.second.target_type() == callback.target_type();
			}),
		m_Callbacks.end()
	);
}

void FGameState::NotifyCallbacks(EGameStateEvent eventType)
{
	for (const auto& [event, callback] : m_Callbacks)
	{
		if (event == eventType)
		{
			callback();
		}
	}
}

// Getters
int FGameState::GetScore() const
{
	return m_Score;
}

int FGameState::GetHiScore() const
{
	return m_HiScore;
}

int FGameState::GetLevel() const
{
	return m_Level;
}

int FGameState::GetBallCount() const
{
	return m_RequiredBallCount;
}

float FGameState::GetCountDownTime() const
{
	return m_CountdownTime;
}

float FGameState::GetSurvivedTime() const
{
	return m_TotalSurvivedTime;
}

float FGameState::GetAddOnTime() const
{
	return m_AdditionalTime;
}

float FGameState::GetChargedBallVelocity() const
{
	return m_BallVelocity;
}

bool FGameState::GetIsLevelUp() const
{
	return m_IsLevelUp;
}

bool FGameState::GetIsGameOver() const
{
	return m_IsGameOver;
}

bool FGameState::GetIsGameStarted() const
{
	return m_IsGameStarted;
}

// String Getters
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

// Helper Functions
float FGameState::GetTimeIncreament(const int ballCount)
{
	return (ballCount * 8.0f);
}

int FGameState::GetRequirementBallCount(const int currentLevel)
{
	return (currentLevel * 5);
}
