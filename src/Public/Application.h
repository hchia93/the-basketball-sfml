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
class b2Actor2D;
class b2Actor2DContactListener;

struct FAppWindowData
{
	unsigned int Width = 1000;
	unsigned int Height = 728;
	
	std::string WindowName = "BasketBallSimulator";

	sf::VideoMode GetVideoModeFromData() { return sf::VideoMode({Width, Height}); }
	const std::string& GetWindowName() { return WindowName; }
	sf::Vector2f GetViewportCenter() { return sf::Vector2f(Width / 2.0f, Height / 2.0f); }
};

class Application
{
public:
	Application();
	~Application();

	int Initialize();
	void BeginPlay();

	sf::RenderWindow& GetAppWindow()	{ return m_AppWindow; }
	std::shared_ptr<b2World> GetWorld() { return m_World; }

	float GetElapsedTime() const { return m_ElapsedTime; }
	void ClearTimer() { m_ElapsedTime = 0.0f; }

private:
	// The core update function, called by the main loop in BeginPlay.
	void UpdateFrame(const float DeltaTime);

	// FTickHandle removal
	float m_ElapsedTime = 0.0f;
	float m_TimeElapsedSinceLastFrame = 0.0f;
	sf::Clock m_FixedUpdateClock;

	static void PivotTick(b2Actor2D* Actor);
	static void WheelTick(b2Actor2D* Actor);
	static void BallTick(b2Actor2D* Actor);
	static void SensorOverlap(b2Actor2D* OverlapActor);

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
	
	//Box2D
	b2Vec2 Gravity; 
	std::shared_ptr<b2World> m_World;
	std::unique_ptr<b2Actor2DContactListener> b2ActorContactListner;

	std::vector<std::unique_ptr<sf::Shape>> RenderShapes;
	std::vector<std::unique_ptr<b2Actor2D>> b2Actors;
	std::vector<std::unique_ptr<b2Actor2D>> m_Balls;

	sf::Vertex AngleIndicators[2];

	bool bRightMousePressed = false;
	bool bMiddleMousePressed = false;

	//////////////////////////////////////////
	//		Cached Pointers
	//////////////////////////////////////////

	sf::RectangleShape* ChargeGaugeMax;		// Cached pointer, no need clear. Cleared via RenderShapes.
	sf::RectangleShape* ChargeGaugeProgress;	// Cached pointer, no need clear. Cleared via RenderShapes.

	b2Actor2D* pivotCache;		// Cached pointer, no need clear. Cleared via b2Actors.
	b2Actor2D* WheelCache;		// Cached pointer, no need clear. Cleared via b2Actors.

	FTextWidget* m_LevelNumberWidget;
	FTextWidget* m_ScoreWidget;
	FTextWidget* m_HiScoreWidget;
	FTextWidget* m_BallCountWidget;
	FTextWidget* m_TimerWidget;
	FTextWidget* m_ElapsedTimeWidget;
	FTextWidget* m_CenterMessageWidget;
};
