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
#include "TextProcessor.h"

// Forward declaration
class Actor;
class ActorContactListener;

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

private:
	void UpdateFrame(const float deltaTime);
	void HandleWindowEvents();
	void UpdateGameSystems();
	void UpdateUIText();
	void HandleInput();
	void RenderFrame();
	void HandleKeyboardInput();
	void HandleMouseInput();
	void UpdateVisualFeedback();
	void ResetGame();

	float m_ElapsedTime = 0.0f;
	float m_TimeElapsedSinceLastFrame = 0.0f;
	sf::Clock m_FixedUpdateClock;

	static void PivotTick(Actor* actor);
	static void WheelTick(Actor* actor);
	static void BallTick(Actor* actor);
	static void SensorOverlap(Actor* overlapActor);

	void MakeTrack();
	void MakeProjector();
	void SetupText();
	void SpawnBall();

	FAssetLoader m_AssetLoader;
	FGameState m_GameState;
	FTextWidgetProcessor m_TextProcessor;
	FAppWindowData m_AppWindowData;
	sf::RenderWindow m_AppWindow;
	sf::Music* m_BGM;

	b2Vec2 m_Gravity;
	std::shared_ptr<b2World> m_World;
	std::unique_ptr<ActorContactListener> m_B2ActorContactListner;
	std::vector<std::unique_ptr<sf::Shape>> m_RenderShapes;
	std::vector<std::unique_ptr<Actor>> m_B2Actors;
	std::vector<std::unique_ptr<Actor>> m_Balls;

	sf::Vertex m_AngleIndicators[2];
	bool m_RightMousePressed = false;
	bool m_MiddleMousePressed = false;

	sf::RectangleShape* m_ChargeGaugeMax;
	sf::RectangleShape* m_ChargeGaugeProgress;

	Actor* m_PivotCache;
	Actor* m_WheelCache;
	
	FTextWidget* m_LevelNumberWidget;
	FTextWidget* m_ScoreWidget;
	FTextWidget* m_HiScoreWidget;
	FTextWidget* m_BallCountWidget;
	FTextWidget* m_TimerWidget;
	FTextWidget* m_ElapsedTimeWidget;
	FTextWidget* m_CenterMessageWidget;
};
