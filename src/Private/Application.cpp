#include <Box2D/Box2D.h>
#include <SFML/Audio.hpp>

#include "Actor.h"
#include "ActorBuilder.h"
#include "ActorContactListener.h"
#include "Application.h"
#include "GameInput.h"
#include "GameLogic.h"
#include "TextWidgetBuilder.h"
#include "TextWidgetProcessor.h"

Application::Application()
{
	m_B2ActorContactListner = std::make_unique<ActorContactListener>();
	m_GameInput = std::make_unique<GameInput>(this);

	m_Gravity = b2Vec2(0.f, 9.81f);
	m_World = std::make_shared<b2World>(m_Gravity);
	m_World->SetContactListener(m_B2ActorContactListner.get());
}

Application::~Application()
{

}

void Application::BeginPlay()
{
	// Main game loop
	while (m_AppWindow.isOpen())
	{
		// Accumulate time since last frame
		m_TimeElapsedSinceLastFrame += m_FixedUpdateClock.restart().asSeconds();
		
		// Fixed timestep physics update (50 FPS)
		if (m_TimeElapsedSinceLastFrame >= DELTA_TIME_STEP)
		{
			// Update physics simulation
			m_World->Step(DELTA_TIME_STEP, 8, 3);

			// Update game logic
			UpdateFrame(DELTA_TIME_STEP);
			
			// Reset accumulated time
			m_TimeElapsedSinceLastFrame -= DELTA_TIME_STEP;
			m_ElapsedTime += DELTA_TIME_STEP;
		}
		
		// Frame limiting to prevent excessive CPU usage
		sf::sleep(sf::milliseconds(1));
	}
}

int Application::Initialize()
{
	// Reduce the code length, scope in this function only.

	bool initChecks = m_AssetLoader.LoadResources();

	if (initChecks)
	{
		if ((m_BGM = m_AssetLoader.FindMusic(RESOURCES_AUDIO_TROLOLO)))
		{
			m_BGM->setVolume(10);
			m_BGM->setLooping(true);
			m_BGM->play();
		}

		// Window creation
		m_AppWindow.create(m_AppWindowData.GetVideoModeFromData(), m_AppWindowData.GetWindowName());
		m_AppWindow.setVerticalSyncEnabled(true);
		//AppWindow.setActive();

		// Border creations
		const float borderThickness = 16.0f;
		const float viewportX = (float)m_AppWindowData.m_Width;
		const float viewportY = (float)m_AppWindowData.m_Height;
		const sf::Vector2f xBorder(viewportX, borderThickness);
		const sf::Vector2f yBorder(borderThickness, viewportY * 0.7f);
		const sf::Vector2f uBorderLocation(viewportX * 0.5f						, borderThickness * 0.5f);
		const sf::Vector2f dBorderLocation(viewportX * 0.5f						, viewportY - borderThickness * 0.5f);
		const sf::Vector2f lBorderLocation(borderThickness * 0.5f				, viewportY * 0.5f - (viewportY * .15f) ); // 1 - .7f div 2
		const sf::Vector2f rBorderLocation(viewportX - borderThickness * 0.5f	, viewportY * 0.5f - (viewportY * .15f) ); // 1 - .7f div 2

		// Create borders using ActorBuilder factory methods
		m_B2Actors.push_back(ActorBuilder::CreateBorder(this, m_World.get(), "TopBorder", xBorder, uBorderLocation));
		m_B2Actors.push_back(ActorBuilder::CreateBorder(this, m_World.get(), "LeftBorder", yBorder, lBorderLocation));
		m_B2Actors.push_back(ActorBuilder::CreateBorder(this, m_World.get(), "RightBorder", yBorder, rBorderLocation));

#if 1 // debug floor!
		m_B2Actors.push_back(ActorBuilder::CreateBorder(this, m_World.get(), "BotBorder", xBorder, dBorderLocation));
#endif 
		std::unique_ptr<sf::RectangleShape> background = std::make_unique<sf::RectangleShape>(sf::Vector2f(viewportX, viewportY));
		background->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_BACKGROUND));
		m_RenderShapes.push_back(std::move(background));

		std::unique_ptr<sf::RectangleShape> scoreboard = std::make_unique<sf::RectangleShape>(sf::Vector2f(viewportX, viewportY * .3f));
		scoreboard->setPosition({0.0f, viewportY * 0.7f});
		scoreboard->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_CHALKBOARD));
		m_RenderShapes.push_back(std::move(scoreboard));

		std::unique_ptr<sf::RectangleShape> chargeGaugeMaxUniquePtr = std::make_unique<sf::RectangleShape>();
		chargeGaugeMaxUniquePtr->setFillColor(sf::Color(145, 145, 145, 255));
		chargeGaugeMaxUniquePtr->setSize(sf::Vector2f(160.0f, 8.0f));
		m_ChargeGaugeMax = chargeGaugeMaxUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		m_RenderShapes.push_back(std::move(chargeGaugeMaxUniquePtr));

		std::unique_ptr<sf::RectangleShape> chargeGaugeProgressUniquePtr = std::make_unique<sf::RectangleShape>();
		chargeGaugeProgressUniquePtr->setFillColor(sf::Color::Yellow);
		chargeGaugeProgressUniquePtr->setFillColor(sf::Color::Yellow);
		m_ChargeGaugeProgress = chargeGaugeProgressUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		m_RenderShapes.push_back(std::move(chargeGaugeProgressUniquePtr));

		for (int i = 0; i < 2; i++)
		{
			m_AngleIndicators[i].color = (i == 1) ? sf::Color::Cyan : sf::Color::Blue;
		}
		
		// Board
		const float offsetX = viewportX * 0.98f;
		const float offsetY = viewportY * 0.35f;
		const sf::Vector2f boardSize(8.0f, 200.0f);
		const sf::Vector2f boardPos(viewportX * 0.98f, viewportY * 0.35f);

		// Board Frame 1
		auto boardFrame1 = ActorBuilder()
			.WithName("board1")
			.WithShape(EActorShapeType::Rectangle)
			.WithBodyType(Eb2ShapeType::Polygon)
			.WithSize(boardSize)
			.WithLocation(boardPos)
			.WithColor(sf::Color(40, 40, 40, 255))
			.WithOutline(sf::Color::Black, -1.0f)
			.Build(this, m_World.get());
		m_B2Actors.push_back(std::move(boardFrame1));

		const sf::Vector2f netEdgeSize(8.0f, 90.0f);
		const sf::Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);
		
		// Board Frame 2
		auto boardFrame2 = ActorBuilder()
			.WithName("board2")
			.WithShape(EActorShapeType::Rectangle)
			.WithBodyType(Eb2ShapeType::Polygon)
			.WithSize(netEdgeSize)
			.WithLocation(netEdgePos)
			.WithColor(sf::Color(40, 40, 40, 255))
			.WithOutline(sf::Color::Black, -1.0f)
			.Build(this, m_World.get());
		boardFrame2->GetBodyInstance()->SetTransform(boardFrame2->GetBodyInstance()->GetPosition(), -0.261799388f);
		m_B2Actors.push_back(std::move(boardFrame2));
	
		const sf::Vector2f sensorSize(48.0f, 48.0f);
		const sf::Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);
		
		// Score Sensor
		auto scoreSensor = ActorBuilder::CreateSensor(this, m_World.get(), "sensor", sensorSize, sensorPos);
		scoreSensor->BindOnBeginoverlap([](Actor* actor) { GameLogic::SensorOverlap(actor); });
		m_B2Actors.push_back(std::move(scoreSensor));

		MakeTrack();
		MakeProjector();
		SetupText();
		SetupInputCallbacks();
	}

	return initChecks;
}

void Application::UpdateFrame(const float deltaTime)
{	
	// Handle window events
	HandleWindowEvents();
	
	// Update game systems
	UpdateGameSystems();
	
	// Handle input via GameInput
	m_GameInput->HandleInput();
	
	// Render everything
	RenderFrame();
}

void Application::HandleWindowEvents()
{
	std::optional<sf::Event> event = m_AppWindow.pollEvent();
	if (event.has_value() && event.value().is<sf::Event::Closed>())
	{
		m_AppWindow.close();
	}
}

void Application::UpdateGameSystems()
{
	m_GameState.Tick();
	m_TextProcessor.Tick();

	for (auto&& element : m_B2Actors)
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
}

void Application::ResetGame()
{
	m_GameState.ResetGame();
	ClearTimer();
	m_CenterMessageWidget->Init();
	m_CenterMessageWidget->m_bIsActive = true;
	m_CenterMessageWidget->m_bIsPaused = true;
	m_PivotCache->ResetToInitTransform();

	for (auto& ball : m_Balls)
	{
		ball->MakeInactive();
	}
}

void Application::RenderFrame()
{
	// Clear the window
	m_AppWindow.clear(CORNFLOWER_BLUE);

	// Render background shapes
	for (auto& element : m_RenderShapes)
	{
		if (element)
		{
			m_AppWindow.draw(*element);
		}
	}

	// Render physics actors
	for (auto& element : m_B2Actors)
	{
		if (element)
		{
			m_AppWindow.draw(*element->GetShape());
		}
	}

	// Render balls
	for (auto& element : m_Balls)
	{
		if (element)
		{
			m_AppWindow.draw(*element->GetShape());
			m_AppWindow.draw(*element->m_DebugForward);
		}
	}

	// Render UI text widgets
	for (auto& widget : m_TextProcessor.GetTextWidgets())
	{
		if(widget && widget->m_bIsActive)
		{
			m_AppWindow.draw(*widget->m_Text);
		}
	}

	// Render angle indicators
	m_AppWindow.draw(m_AngleIndicators, 2, sf::PrimitiveType::Lines);
	
	// Display everything
	m_AppWindow.display();
}

void Application::MakeTrack()
{
	// The Track 
	const float viewportX = (float)m_AppWindowData.m_Width;
	const float viewportY = (float)m_AppWindowData.m_Height;
	const int row = 14;
	const int column = 2;

	const sf::Vector2f startLocation(viewportX * 0.15f, viewportY - 16.0f - (row*32.0f));
	const sf::Vector2f size(32.0f, 32.0f);

	std::unique_ptr<sf::RectangleShape> tracks[row][column];
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < column; ++j)
		{
			tracks[i][j] = std::make_unique<sf::RectangleShape>();

			tracks[i][j]->setSize(size);
			tracks[i][j]->setPosition({startLocation.x + j * 32.0f, startLocation.y + i * 32.0f});
			tracks[i][j]->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_BOXALT));
			m_RenderShapes.push_back(std::move(tracks[i][j]));
		}
	}
}

void Application::MakeProjector()
{
	// Projector Pivot
	const int row = 14;
	const float viewportX = (float)m_AppWindowData.m_Width;
	const float viewportY = (float)m_AppWindowData.m_Height;
	const sf::Vector2f startLocation(viewportX * 0.15f, viewportY - 16.0f - (row * 32.0f));
	const sf::Vector2f location(startLocation.x + 32.0f, startLocation.y + (row / 2)*32.0f);
	
	auto pivot = ActorBuilder()
		.WithName("Pivot")
		.WithShape(EActorShapeType::Rectangle)
		.WithBodyType(Eb2ShapeType::Polygon)
		.WithSize(sf::Vector2f(8.0f, 8.0f))
		.WithLocation(location)
		.WithColor(sf::Color(0, 0, 255, 100))
		.WithOutline(sf::Color::Black, -1.0f)
		.Build(this, m_World.get());
	pivot->BindOnTick([](Actor* actor) { GameLogic::PivotTick(actor); });
	m_PivotCache = pivot.get();
	m_B2Actors.push_back(std::move(pivot));

	// Projector Wheel
	const sf::Vector2f wheelLocation(location.x - 24, location.y);
	auto wheel = ActorBuilder()
		.WithName("Wheel")
		.WithShape(EActorShapeType::Circle)
		.WithBodyType(Eb2ShapeType::Polygon)
		.WithSize(sf::Vector2f(48.0f, 48.0f))
		.WithLocation(wheelLocation)
		.WithColor(sf::Color(0, 255, 255, 40))
		.WithOutline(sf::Color::Black, -1.0f)
		.Build(this, m_World.get());
	wheel->GetShape()->setOrigin(wheel->GetShape()->getOrigin() + sf::Vector2f(24, 0));
	wheel->BindOnTick([](Actor* actor) { GameLogic::WheelTick(actor); });
	m_B2Actors.push_back(std::move(wheel));
}

void Application::SetupText()
{
	auto pChalkFont = std::shared_ptr<sf::Font>(m_AssetLoader.FindFont(RESOURCES_FONT_CHALK), [](sf::Font*) {});
	auto pPixelFont = std::shared_ptr<sf::Font>(m_AssetLoader.FindFont(RESOURCES_FONT_PIXEL), [](sf::Font*) {});

	const float row1 = 530;
	const float row2 = 620;
	
	// Create text widgets using TextWidgetBuilder factory methods
	m_LevelNumberWidget = TextWidgetBuilder::CreateScoreWidget(m_TextProcessor, pChalkFont, sf::Vector2f(80, row1), "LEVEL\n", [this]() { return m_GameState.GetLevelString(); });
	m_ScoreWidget = TextWidgetBuilder::CreateScoreWidget(m_TextProcessor, pChalkFont, sf::Vector2f(80, row2), "SCORE\n", [this]() { return m_GameState.GetScoreString(); });
	m_HiScoreWidget = TextWidgetBuilder::CreateScoreWidget(m_TextProcessor, pChalkFont, sf::Vector2f(768, row1), "HISCORE\n", [this]() { return m_GameState.GetHiScoreString(); });
	m_BallCountWidget = TextWidgetBuilder::CreateScoreWidget(m_TextProcessor, pChalkFont, sf::Vector2f(768, row2), "REQ. BALL\n", [this]() { return m_GameState.GetReqBallString(); });
	
	m_TimerWidget = TextWidgetBuilder::CreateTimerWidget(m_TextProcessor, pChalkFont, sf::Vector2f(368, row1), "REMAINING TIME\n", [this]() { return m_GameState.GetRemainingTimeString(); });
	
	// Elapsed Time Widget (custom setup due to complex callback)
	m_ElapsedTimeWidget = TextWidgetBuilder()
		.WithFont(pChalkFont)
		.WithSize(30)
		.AtPosition(sf::Vector2f(368, row2))
		.WithPrefix("ELAPSED MIN\n")
		.WithSuffix(" S")
		.WithCallback([this]() { return m_GameState.GetElapsedTimeMinString() + " M" + m_GameState.GetElapsedTimeSecondString(); })
		.BuildAndAdd(m_TextProcessor);

	// Center Message Widget
	m_CenterMessageWidget = TextWidgetBuilder::CreateMessageWidget(m_TextProcessor, pPixelFont, m_AppWindowData.GetViewportCenter(), "PRESS SPACE BAR TO START", 1.0f, true);
}

void Application::SpawnBall()
{
	// Get magnitude of the multiplier.
	const float velocity = m_GameState.GetChargedBallVelocity();
	const sf::Vector2f ballSpawnLocation(m_PivotCache->GetLocation() + sf::Vector2f(32, 32));

	auto findPredicate = [](auto& p)->bool { return !p->IsActive(); };
	auto pActor = std::find_if(m_Balls.begin(), m_Balls.end(), findPredicate);

	// If found, reuse existing ball
	if (pActor != m_Balls.end())
	{
		Actor* const reuseBall = (*pActor) ? (*pActor).get() : nullptr;
		if (reuseBall)
		{
			reuseBall->GetBodyInstance()->SetLinearVelocity(b2Vec2(velocity, -velocity));
			std::cout << "X: "<<reuseBall->GetBodyInstance()->GetLinearVelocity().x << " Y: "<<reuseBall->GetBodyInstance()->GetLinearVelocity().y << std::endl;
			reuseBall->SetInitTransform(ballSpawnLocation, 0.0f);
			reuseBall->ResetToInitTransform();
			reuseBall->Activate();
		}
	}
	else
	{
		// Create new ball using ActorBuilder
		auto ball = ActorBuilder::CreateBall(this, m_World.get(), "Ball", sf::Vector2f(32, 32), ballSpawnLocation);
		ball->GetBodyInstance()->SetLinearVelocity(b2Vec2(velocity , -velocity));
		std::cout << "X: " << ball->GetBodyInstance()->GetLinearVelocity().x << " Y: " << ball->GetBodyInstance()->GetLinearVelocity().y << std::endl;
		ball->GetFixtureDefinition()->density = 0.83f;
		ball->GetFixtureDefinition()->friction = 0.4f;
		ball->GetFixtureDefinition()->restitution = 0.65f;
		ball->BindOnTick([](Actor* actor) { GameLogic::BallTick(actor); });
		m_Balls.push_back(std::move(ball));
	}
}

void Application::SetupInputCallbacks()
{
	// Space bar callback
	m_GameInput->SetOnSpacePressed([this]() {
		if (!m_GameState.GetIsGameStarted())
		{
			m_GameState.StartGame();
			m_CenterMessageWidget->m_bIsPaused = false;
		}
	});

	// Left mouse button callbacks
	m_GameInput->SetOnLeftMousePressed([this]() {
		if (!m_GameState.GetIsGameOver() && m_GameState.GetIsGameStarted())
		{
			m_GameState.ChargeProjectionVelocity();
		}
	});

	m_GameInput->SetOnLeftMouseReleased([this]() {
		m_GameState.DischargeProjectionVelocity();
	});

	// Right mouse button callback
	m_GameInput->SetOnRightMousePressed([this]() {
		if (!m_GameState.GetIsGameOver() && m_GameState.GetIsGameStarted())
		{
			SpawnBall();
		}
	});

	// Middle mouse button callback
	m_GameInput->SetOnMiddleMousePressed([this]() {
		ResetGame();
	});

	// Visual feedback callback
	m_GameInput->SetOnUpdateVisualFeedback([this]() {
		UpdateVisualFeedback();
	});
}

void Application::UpdateVisualFeedback()
{
	// Update charge gauge
	const float maxVelocity = 60.0f;
	const float percentage = m_GameState.GetChargedBallVelocity() / maxVelocity;

	sf::Vector2i mouseLocation = sf::Mouse::getPosition(m_AppWindow);
	const sf::Vector2f offsetMouseLocation = sf::Vector2f(mouseLocation - sf::Vector2i(16, 16));

	m_ChargeGaugeMax->setPosition(offsetMouseLocation);
	m_ChargeGaugeMax->setSize({160.0f, 8.0f});
	m_ChargeGaugeProgress->setPosition(offsetMouseLocation);
	m_ChargeGaugeProgress->setSize(sf::Vector2f(160.0f * percentage, 8.0f));

	// Update angle indicator
	m_AngleIndicators[0].position = m_PivotCache->GetLocation();
	m_AngleIndicators[1].position = sf::Vector2f(mouseLocation);
}
