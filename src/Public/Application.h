#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <box2d/box2d.h>
#include "TickHandle.h"
#include "AssetLoader.h"
#include "GameState.h"
#include "TextRenderer.h"
#include "Defines.h"

class b2Actor2D;
class b2Actor2DContactListener;

struct FRenderWindowData
{
	int m_Width = 1000;
	int m_Height = 728;
	int m_BitsPerPixel;
	
	std::string m_WindowName = "BasketBallSimulator";

	sf::VideoMode GetVideoModeFromData()	{	return sf::VideoMode(m_Width, m_Height, m_BitsPerPixel);	}
	std::string GetWindowName()			{	return m_WindowName;	}
};

class Application
{
public:
	Application();
	~Application();;

	int Initialize();
	void BeginPlay();
	void Tick(const float deltaTime);
	void EndPlay();

	b2World* GetWorld() const { return m_World.get(); }
	FTickHandle& GetTickHandle() { return m_TickHandle;  }
	sf::RenderWindow* GetWindow() { return &m_AppWindow; }

private:

	static void PivotTick(b2Actor2D* actor);
	static void WheelTick(b2Actor2D* actor);
	static void BallTick(b2Actor2D* actor);
	static void SensorOverlap(b2Actor2D* overlapActor);

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
	
	//Box2D
	b2Vec2 m_Gravity; 
	std::shared_ptr<b2World> m_World;
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

	sf::RectangleShape* m_ChargeGaugeMax;		// Cached pointer, no need clear. Cleared via m_RenderShapes.
	sf::RectangleShape* m_ChargeGaugeProgress;	// Cached pointer, no need clear. Cleared via m_RenderShapes.

	b2Actor2D* m_PivotCache;		// Cached pointer, no need clear. Cleared via m_b2Actors.
	b2Actor2D* m_WheelCache;		// Cached pointer, no need clear. Cleared via m_b2Actors.

	FTextData* m_LevelTextCache;
	FTextData* m_ScoreCache;
	FTextData* m_HiScoreCache;
	FTextData* m_BallCountCache;
	FTextData* m_CountdownTimeCache;
	FTextData* m_ElapsedTimeCache;
	FTextData* m_CenterTextCache;

	
};
