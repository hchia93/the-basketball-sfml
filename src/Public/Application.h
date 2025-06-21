#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <box2d/box2d.h>
#include <memory>
#include <vector>
#include "TickHandle.h"
#include "AssetLoader.h"
#include "GameState.h"
#include "TextRenderer.h"
#include "Defines.h"
#include "Box2DHelper.h"

class b2Actor2D;
class b2Actor2DContactListener;

struct FRenderWindowData
{
	sf::Vector2u m_WindowSize = { 1000, 728 };
	std::string m_WindowName = "The BasketBall";
	
	sf::VideoMode GetVideoModeFromData()
	{
		return sf::VideoMode(m_WindowSize);
	}
};

class Application
{
public:
	Application();
	~Application();

	int Initialize();
	void BeginPlay();
	void Tick(const float DeltaTime);
	void EndPlay();

	b2WorldId GetWorld() const { return m_WorldId; }
	FTickHandle& GetTickHandle() { return m_TickHandle; }
	sf::RenderWindow* GetWindow() { return &m_AppWindow; }

private:
	static void PivotTick(b2Actor2D* Actor);
	static void WheelTick(b2Actor2D* Actor);
	static void BallTick(b2Actor2D* Actor);
	static void SensorOverlap(b2Actor2D* OverlapActor);

	void MakeTrack();
	void MakeProjector();
	void SetupText();
	void SpawnBall();

	FTickHandle m_TickHandle;
	FAssetLoader m_AssetLoader;
	FGameState m_GameState;
	FTextRenderer m_TextRenderer;

	FRenderWindowData m_RenderWindowData;
	sf::RenderWindow m_AppWindow;

	sf::Music* m_BGM;
	
	// Box2D 3.x
	b2Vec2 m_Gravity; 
	b2WorldId m_WorldId;
	std::unique_ptr<b2Actor2DContactListener> m_b2ActorContactListner;

	std::vector<std::unique_ptr<sf::Shape>> m_RenderShapes;
	std::vector<std::unique_ptr<b2Actor2D>> m_b2Actors;
	std::vector<std::unique_ptr<b2Actor2D>> m_Balls;

	sf::Vertex m_AngleIndicators[2];

	bool m_bRightMousePressed = false;
	bool m_bMiddleMousePressed = false;

	//////////////////////////////////////////
	//		Cached Pointers
	//////////////////////////////////////////

	sf::RectangleShape* m_ChargeGaugeMax;		// Cached pointer, no need clear. Cleared via RenderShapes.
	sf::RectangleShape* m_ChargeGaugeProgress;	// Cached pointer, no need clear. Cleared via RenderShapes.

	b2Actor2D* m_PivotCache;		// Cached pointer, no need clear. Cleared via b2Actors.
	b2Actor2D* m_WheelCache;		// Cached pointer, no need clear. Cleared via b2Actors.

	FTextData* m_LevelTextCache;
	FTextData* m_ScoreCache;
	FTextData* m_HiScoreCache;
	FTextData* m_BallCountCache;
	FTextData* m_CountdownTimeCache;
	FTextData* m_ElapsedTimeCache;
	FTextData* m_CenterTextCache;
};
