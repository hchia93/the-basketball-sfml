#include <SFML/Audio.hpp>

#include "Application.h"
#include "b2Actor2D.h"
#include "b2Actor2DContactListener.h"

Application::Application()
{
	m_b2ActorContactListner = std::make_unique<b2Actor2DContactListener>();
	m_Gravity = b2Vec2(0.f, 9.81f);
	m_World = std::make_shared<b2World>(m_Gravity);
	m_World->SetContactListener(m_b2ActorContactListner.get());
}

Application::~Application()
{
}

void Application::BeginPlay()
{
	while (m_AppWindow.isOpen())
	{
		m_TickHandle.BeginTick();
	}

	EndPlay();
}

int Application::Initialize()
{
	bool bInitChecks = true;
	bInitChecks &= m_TickHandle.BindApplication(this);
	bInitChecks &= m_GameState.BindApplication(this);
	bInitChecks &= m_TextRenderer.BindApplication(this);
	bInitChecks &= m_AssetLoader.LoadResources();

	if (bInitChecks)
	{
		m_BGM = FAssetLoader::FindMusic(&m_AssetLoader, RESOURCES_AUDIO_TROLOLO);
		if (m_BGM)
		{
			m_BGM->setVolume(0);
			m_BGM->setLoop(true);
			m_BGM->play();
		}

		m_AppWindow.create(m_RenderWindowData.GetVideoModeFromData(), m_RenderWindowData.GetWindowName());
		m_AppWindow.setVerticalSyncEnabled(true);
		m_AppWindow.setActive();

		const float borderThickness = 16.0f;
		const float viewportX = (float)m_RenderWindowData.m_Width;
		const float viewportY = (float)m_RenderWindowData.m_Height;
		const sf::Vector2f xBorder(viewportX, borderThickness);
		const sf::Vector2f yBorder(borderThickness, viewportY * 0.7f);
		const sf::Vector2f uBorderLocation(viewportX * 0.5f, borderThickness * 0.5f);
		const sf::Vector2f dBorderLocation(viewportX * 0.5f, viewportY - borderThickness * 0.5f);
		const sf::Vector2f lBorderLocation(borderThickness * 0.5f, viewportY * 0.5f - (viewportY * .15f));
		const sf::Vector2f rBorderLocation(viewportX - borderThickness * 0.5f, viewportY * 0.5f - (viewportY * .15f));

		auto b2ActorInit = [this](std::unique_ptr<b2Actor2D>& p, const sf::Color c) -> void
		{
			p->GetShape()->setOutlineThickness(-1);
			p->GetShape()->setOutlineColor(sf::Color::Black);
			p->GetShape()->setFillColor(c);
			m_b2Actors.push_back(std::move(p));
		};

		auto topBorder = std::make_unique<b2Actor2D>(this, m_World.get(), "TopBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, xBorder, uBorderLocation);
		b2ActorInit(topBorder, sf::Color(100, 100, 100));

		auto leftBorder = std::make_unique<b2Actor2D>(this, m_World.get(), "LeftBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, yBorder, lBorderLocation);
		b2ActorInit(leftBorder, sf::Color(100, 100, 100));

		auto rightBorder = std::make_unique<b2Actor2D>(this, m_World.get(), "RightBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, yBorder, rBorderLocation);
		b2ActorInit(rightBorder, sf::Color(100, 100, 100));

#if 1 // debug floor!
		auto botBorder = std::make_unique<b2Actor2D>(this, m_World.get(), "BotBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, xBorder, dBorderLocation);
		b2ActorInit(botBorder, sf::Color(100, 100, 100));
#endif
		auto background = std::make_unique<sf::RectangleShape>(sf::Vector2f(viewportX, viewportY));
		background->setTexture(FAssetLoader::FindTexture(&m_AssetLoader, RESOURCES_TEXTURE_BACKGROUND));
		m_RenderShapes.push_back(std::move(background));

		auto scoreboard = std::make_unique<sf::RectangleShape>(sf::Vector2f(viewportX, viewportY * .3f));
		scoreboard->setPosition(0.0f, viewportY * 0.7f);
		scoreboard->setTexture(FAssetLoader::FindTexture(&m_AssetLoader, RESOURCES_TEXTURE_CHALKBOARD));
		m_RenderShapes.push_back(std::move(scoreboard));

		auto chargeGaugeMaxUniquePtr = std::make_unique<sf::RectangleShape>();
		chargeGaugeMaxUniquePtr->setFillColor(sf::Color(145, 145, 145, 255));
		chargeGaugeMaxUniquePtr->setSize(sf::Vector2f(160.0f, 8.0f));
		m_ChargeGaugeMax = chargeGaugeMaxUniquePtr.get();
		m_RenderShapes.push_back(std::move(chargeGaugeMaxUniquePtr));

		auto chargeGaugeProgressUniquePtr = std::make_unique<sf::RectangleShape>();
		chargeGaugeProgressUniquePtr->setFillColor(sf::Color::Yellow);
		m_ChargeGaugeProgress = chargeGaugeProgressUniquePtr.get();
		m_RenderShapes.push_back(std::move(chargeGaugeProgressUniquePtr));

		for (int i = 0; i < 2; i++)
		{
			m_AngleIndicators[i].color = (i == 1) ? sf::Color::Cyan : sf::Color::Blue;
		}

		const float offsetX = viewportX * 0.98f;
		const float offsetY = viewportY * 0.35f;
		const sf::Vector2f boardSize(8.0f, 200.0f);
		const sf::Vector2f boardPos(viewportX * 0.98f, viewportY * 0.35f);

		auto boardFrame1 = std::make_unique<b2Actor2D>(this, m_World.get(), "board1", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, boardSize, boardPos);
		b2ActorInit(boardFrame1, sf::Color(40, 40, 40, 255));

		const sf::Vector2f netEdgeSize(8.0f, 90.0f);
		const sf::Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);

		auto boardFrame2 = std::make_unique<b2Actor2D>(this, m_World.get(), "board2", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, netEdgeSize, netEdgePos);
		boardFrame2->GetBodyInstance()->SetTransform(boardFrame2->GetBodyInstance()->GetPosition(), -0.261799388f);
		b2ActorInit(boardFrame2, sf::Color(40, 40, 40, 255));

		const sf::Vector2f sensorSize(48.0f, 48.0f);
		const sf::Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);

		auto scoreSensor = std::make_unique<b2Actor2D>(this, m_World.get(), "sensor", EActorShapeType::Circle, Eb2ShapeType::Circle, sensorSize, sensorPos, 0.0f, false, true);
		scoreSensor->BindOnBeginoverlap(SensorOverlap);
		b2ActorInit(scoreSensor, sf::Color(255, 255, 0, 100));

		MakeTrack();
		MakeProjector();
		SetupText();
	}

	return bInitChecks;
}

void Application::Tick(const float deltaTime)
{
	sf::Event exitEvent;
	while (m_AppWindow.pollEvent(exitEvent))
	{
		if (exitEvent.type == sf::Event::Closed)
		{
			m_AppWindow.close();
		}
	}

	m_GameState.Tick();
	m_TextRenderer.Tick();

	for (auto& element : m_b2Actors)
	{
		if (element)
		{
			element->Tick();
		}
	}

	for (auto& element : m_Balls)
	{
		if (element)
		{
			element->Tick();
		}
	}

	m_LevelTextCache->m_Text.setString("LEVEL\n" + m_GameState.GetLevelString());
	m_ScoreCache->m_Text.setString("SCORE\n" + m_GameState.GetScoreString());
	m_HiScoreCache->m_Text.setString("HISCORE\n" + m_GameState.GetHiScoreString());
	m_BallCountCache->m_Text.setString("REQ. BALL\n" + m_GameState.GetReqBallString());
	m_CountdownTimeCache->m_Text.setString("REMAINING TIME\n" + m_GameState.GetRemainingTimeString() + " S");
	m_ElapsedTimeCache->m_Text.setString("ELAPSED MIN\n" + m_GameState.GetElapsedTimeMinString() + " M" + m_GameState.GetElapsedTimeSecondString() + " S");

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		if (!m_GameState.GetIsGameStarted())
		{
			m_GameState.StartGame();
			m_CenterTextCache->m_bIsPaused = false;
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (!m_GameState.GetIsGameOver() && m_GameState.GetIsGameStarted())
		{
			m_GameState.ChargeProjectionVelocity();
		}
	}
	else
	{
		m_GameState.DischargeProjectionVelocity();
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		if (!m_bRightMousePressed)
		{
			if (!m_GameState.GetIsGameOver() && m_GameState.GetIsGameStarted())
			{
				SpawnBall();
			}
			m_bRightMousePressed = true;
		}
	}
	else
	{
		m_bRightMousePressed = false;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
	{
		if (!m_bMiddleMousePressed)
		{
			m_bMiddleMousePressed = true;

			m_GameState.ResetGame();
			m_TickHandle.ClearTimer();
			m_CenterTextCache->Init();
			m_CenterTextCache->m_bIsActive = true;
			m_CenterTextCache->m_bIsPaused = true;
			m_PivotCache->ResetToInitTransform();
			m_WheelCache->ResetToInitTransform();

			for (auto& element : m_Balls)
			{
				if (element)
				{
					element->MakeInactive();
				}
			}
		}
	}
	else
	{
		m_bMiddleMousePressed = false;
	}

	float maxVelocity = 60.0f;
	float percentage = m_GameState.GetChargedBallVelocity() / maxVelocity;

	const sf::Vector2f pivotLocation = m_PivotCache->GetLocation();
	const sf::Vector2f mouseLocation = sf::Vector2f(sf::Mouse::getPosition(m_AppWindow));
	const sf::Vector2f offsetMouseLocation = sf::Vector2f(sf::Mouse::getPosition(m_AppWindow) - sf::Vector2i(16, 16));

	m_ChargeGaugeMax->setPosition(offsetMouseLocation);
	m_ChargeGaugeMax->setSize(sf::Vector2f(160.0f, 8.0f));
	m_ChargeGaugeProgress->setPosition(offsetMouseLocation);
	m_ChargeGaugeProgress->setSize(sf::Vector2f(160.0f * percentage, 8.0f));

	const sf::Vector2f direction = pivotLocation - mouseLocation;
	const float rotation = (atan2(direction.y, direction.x)) * 180.0f / (float)PI;
	m_PivotCache->GetBodyInstance()->SetTransform(m_PivotCache->GetBodyInstance()->GetPosition(), rotation * (float)PI / 180.0f);
	m_WheelCache->GetBodyInstance()->SetTransform(m_WheelCache->GetBodyInstance()->GetPosition(), rotation * (float)PI / 180.0f);

	m_AngleIndicators[0].position = pivotLocation;
	m_AngleIndicators[1].position = mouseLocation;

	m_AppWindow.clear();
	for (const auto& element : m_RenderShapes)
	{
		if (element)
		{
			m_AppWindow.draw(*element);
		}
	}
	for (const auto& element : m_b2Actors)
	{
		if (element && element->IsActive())
		{
			m_AppWindow.draw(*(element->GetShape()));
		}
	}
	for (const auto& element : m_Balls)
	{
		if (element && element->IsActive())
		{
			m_AppWindow.draw(*(element->GetShape()));
			m_AppWindow.draw(*(element->m_DebugForward));
		}
	}
	for (const auto& element : m_TextRenderer.GetTextData())
	{
		if (element && element->m_bIsActive)
		{
			m_AppWindow.draw(element->m_Text);
		}
	}

	m_AppWindow.draw(m_AngleIndicators, 2, sf::Lines);
	m_AppWindow.display();
}

void Application::EndPlay()
{
	m_TickHandle.EndTick();
}

void Application::PivotTick(b2Actor2D* actor)
{
	if (actor)
	{
		const float angularVelocity = -20.0f;
		actor->GetBodyInstance()->SetAngularVelocity(angularVelocity);
	}
}

void Application::WheelTick(b2Actor2D* actor)
{
	if (actor)
	{
		const float angularVelocity = 20.0f;
		actor->GetBodyInstance()->SetAngularVelocity(angularVelocity);
	}
}

void Application::BallTick(b2Actor2D* actor)
{
	if (actor)
	{
		if (actor->GetBodyInstance()->GetPosition().y > 100)
		{
			actor->MakeInactive();
		}
	}
}

void Application::SensorOverlap(b2Actor2D* overlapActor)
{
	if (overlapActor && overlapActor->GetObjectName() == "ball")
	{
		FGameState* gameState = (FGameState*)(&(static_cast<Application*>(overlapActor->GetPackage())->m_GameState));
		if (gameState)
		{
			gameState->ScoreBall();
		}
	}
}

void Application::MakeTrack()
{
	const float viewportX = (float)m_RenderWindowData.m_Width;
	const float viewportY = (float)m_RenderWindowData.m_Height;

	auto setup = [this](std::unique_ptr<b2Actor2D>& p, const sf::Color c) -> void
	{
		p->GetShape()->setOutlineThickness(-1);
		p->GetShape()->setOutlineColor(sf::Color::Black);
		p->GetShape()->setFillColor(c);
		m_b2Actors.push_back(std::move(p));
	};

	const int step = 32;
	for (int i = 1; i < step; ++i)
	{
		const float t = (float)i / (float)(step - 1);
		const float x = lerp(viewportX * 0.25f, viewportX * 0.75f, t);
		const float y = viewportY * 0.5f + sin(t * PI * 2.0f) * 100.0f;

		auto p = std::make_unique<b2Actor2D>(this, m_World.get(), "track", EActorShapeType::Circle, Eb2ShapeType::Circle, sf::Vector2f(16.0f, 16.0f), sf::Vector2f(x, y));
		setup(p, sf::Color(255, 100, 100, 255));
	}
}

void Application::MakeProjector()
{
	auto b2ActorInit = [this](std::unique_ptr<b2Actor2D>& p, const sf::Color c) -> void
	{
		p->GetShape()->setOutlineThickness(-1);
		p->GetShape()->setOutlineColor(sf::Color::Black);
		p->GetShape()->setFillColor(c);
		m_b2Actors.push_back(std::move(p));
	};

	const sf::Vector2f pivotPos(100, 400);
	auto pivot = std::make_unique<b2Actor2D>(this, m_World.get(), "pivot", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, sf::Vector2f(100, 8), pivotPos, 0.0f, true);
	b2Body* pivotBody = pivot->GetBodyInstance();
	pivot->BindOnTick(PivotTick);
	b2ActorInit(pivot, sf::Color(100, 100, 100, 255));
	m_PivotCache = pivot.get();

	auto wheel = std::make_unique<b2Actor2D>(this, m_World.get(), "wheel", EActorShapeType::Circle, Eb2ShapeType::Circle, sf::Vector2f(32, 32), pivotPos, 0.0f, true);
	b2Body* wheelBody = wheel->GetBodyInstance();
	b2ActorInit(wheel, sf::Color(100, 255, 100, 255));
	m_WheelCache = wheel.get();

	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = pivotBody;
	revoluteJointDef.bodyB = wheelBody;
	revoluteJointDef.collideConnected = false;
	revoluteJointDef.localAnchorA.Set(0, 0);
	revoluteJointDef.localAnchorB.Set(0, 0);
	m_World->CreateJoint(&revoluteJointDef);
}

void Application::SetupText()
{
	sf::Font* pixelFont = FAssetLoader::FindFont(&m_AssetLoader, RESOURCES_FONT_PIXEL);
	sf::Font* chalkFont = FAssetLoader::FindFont(&m_AssetLoader, RESOURCES_FONT_CHALK);

	if (pixelFont)
	{
		auto addText = [this, pixelFont](const std::string& text, const sf::Vector2f& pos, FTextData*& cache)
		{
			auto data = std::make_unique<FTextData>();
			data->m_Text.setFont(*pixelFont);
			data->m_Text.setString(text);
			data->m_Text.setPosition(pos);
			data->m_Text.setOutlineThickness(1.0f);
			cache = data.get();
			m_TextRenderer.Add(data);
		};

		addText("LEVEL", sf::Vector2f(100, 640), m_LevelTextCache);
		addText("SCORE", sf::Vector2f(250, 640), m_ScoreCache);
		addText("HISCORE", sf::Vector2f(400, 640), m_HiScoreCache);
		addText("REQ. BALL", sf::Vector2f(550, 640), m_BallCountCache);
		addText("REMAINING TIME", sf::Vector2f(100, 680), m_CountdownTimeCache);
		addText("ELAPSED MIN", sf::Vector2f(400, 680), m_ElapsedTimeCache);
	}

	if (chalkFont)
	{
		auto data = std::make_unique<FTextData>();
		data->m_Text.setFont(*chalkFont);
		data->m_Text.setString("GET READY");
		data->m_Text.setCharacterSize(80);
		data->m_Text.setPosition(250, 250);
		data->m_StartLocation = sf::Vector2f(250, 250);
		data->m_EndLocation = sf::Vector2f(250, 0);
		data->m_bIsPersistance = false;
		data->m_FadeTime = 1.5f;
		data->m_bIsPaused = true;
		m_CenterTextCache = data.get();
		m_TextRenderer.Add(data);
	}
}

void Application::SpawnBall()
{
	const float ballSize = 32.0f;
	auto ball = std::make_unique<b2Actor2D>(this, m_World.get(), "ball", EActorShapeType::Circle, Eb2ShapeType::Circle, sf::Vector2f(ballSize, ballSize), m_PivotCache->GetLocation(), m_PivotCache->GetBodyInstance()->GetAngle(), true);
	ball->GetShape()->setTexture(FAssetLoader::FindTexture(&m_AssetLoader, RESOURCES_TEXTURE_BASKETBALL));
	ball->BindOnTick(BallTick);

	const float rad = ball->GetBodyInstance()->GetAngle();
	const b2Vec2 force(cos(rad) * m_GameState.GetChargedBallVelocity(), sin(rad) * m_GameState.GetChargedBallVelocity());
	ball->GetBodyInstance()->ApplyLinearImpulseToCenter(force, true);

	m_Balls.push_back(std::move(ball));
}

// Implementations for functions moved from header
sf::VideoMode FRenderWindowData::GetVideoModeFromData() 
{ 
	return sf::VideoMode(m_Width, m_Height, m_BitsPerPixel); 
}

std::string FRenderWindowData::GetWindowName() 
{ 
	return m_WindowName; 
}

b2World* Application::GetWorld() const
{ 
	return m_World.get(); 
}

FTickHandle& Application::GetTickHandle() 
{ 
	return m_TickHandle; 
}

sf::RenderWindow* Application::GetWindow() 
{ 
	return &m_AppWindow; 
}
