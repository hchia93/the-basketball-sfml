#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>
#include <string>
#include <vector>
#include <memory>
#include "Defines.h"
#include "AssetLoader.h"
#include "GameState.h"
#include "TextWidgetProcessor.h"

// Forward declaration
class Actor;
class ActorContactListener;
class GameInput;

struct FAppWindowData
{
	unsigned int m_Width = 1000;
	unsigned int m_Height = 728;
	std::string m_WindowName = "BasketBallSimulator";

	sf::VideoMode GetVideoModeFromData() { return sf::VideoMode({m_Width, m_Height}); }
	const std::string& GetWindowName() { return m_WindowName; }
	sf::Vector2f GetViewportCenter() { return sf::Vector2f(m_Width / 2.0f, m_Height / 2.0f); }
};

class Application
{
public:
	Application();
	~Application();

	int Initialize();
	void BeginPlay();

	sf::RenderWindow& GetAppWindow() { return m_AppWindow; }
	std::shared_ptr<b2World> GetWorld() { return m_World; }

	float GetElapsedTime() const { return m_ElapsedTime; }
	void ClearTimer() { m_ElapsedTime = 0.0f; }

	// Public getters for private members needed by GameLogic
	FGameState& GetGameState() { return m_GameState; }
	const FAppWindowData& GetAppWindowData() const { return m_AppWindowData; }
	Actor* GetPivotCache() const { return m_PivotCache; }
	FAssetLoader& GetAssetLoader() { return m_AssetLoader; }
	GameInput* GetGameInput() { return m_GameInput.get(); }

private:
	void UpdateFrame(const float deltaTime);
	void HandleWindowEvents();
	void UpdateGameSystems();
	void RenderFrame();
	void ResetGame();

	float m_ElapsedTime = 0.0f;
	float m_TimeElapsedSinceLastFrame = 0.0f;
	sf::Clock m_FixedUpdateClock;

	void MakeTrack();
	void MakeProjector();
	void SetupText();
	void SpawnBall();
	void SetupInputCallbacks();
	void UpdateVisualFeedback();

	FAssetLoader m_AssetLoader;
	FGameState m_GameState;
	FTextWidgetProcessor m_TextProcessor;
	FAppWindowData m_AppWindowData;
	sf::RenderWindow m_AppWindow;
	sf::Music* m_BGM;
	std::unique_ptr<GameInput> m_GameInput;

	b2Vec2 m_Gravity;
	std::shared_ptr<b2World> m_World;
	std::unique_ptr<ActorContactListener> m_B2ActorContactListner;
	std::vector<std::unique_ptr<sf::Shape>> m_RenderShapes;
	std::vector<std::unique_ptr<Actor>> m_B2Actors;
	std::vector<std::unique_ptr<Actor>> m_Balls;

	sf::Vertex m_AngleIndicators[2];

	sf::RectangleShape* m_ChargeGaugeMax;
	sf::RectangleShape* m_ChargeGaugeProgress;

	Actor* m_PivotCache;
	
	FTextWidget* m_LevelNumberWidget;
	FTextWidget* m_ScoreWidget;
	FTextWidget* m_HiScoreWidget;
	FTextWidget* m_BallCountWidget;
	FTextWidget* m_TimerWidget;
	FTextWidget* m_ElapsedTimeWidget;
	FTextWidget* m_CenterMessageWidget;
};
