#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>

#include "Application.h"
#include "Actor.h"
#include "ActorContactListener.h"

Application::Application()
{
	m_B2ActorContactListner = std::make_unique<ActorContactListener>();

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

		// Collapsed function body. Transfering ownership of local unique ptr to the container
		auto b2ActorInit = [this](std::unique_ptr<Actor>& actor, const sf::Color& color) ->void 
		{
			actor->GetShape()->setOutlineThickness(-1);
			actor->GetShape()->setOutlineColor(sf::Color::Black);
			actor->GetShape()->setFillColor(color);
			m_B2Actors.push_back(std::move(actor));
		};

		// Top Border
		std::unique_ptr<Actor> topBorder = std::make_unique<Actor>(this, m_World.get(), "TopBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, xBorder, uBorderLocation, 0.0f, false, false, true);
		b2ActorInit(topBorder, sf::Color(100, 100, 100));

		// Left Border
		std::unique_ptr<Actor> leftBorder = std::make_unique<Actor>(this, m_World.get(), "LeftBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, yBorder, lBorderLocation, 0.0f, false, false, true);
		b2ActorInit(leftBorder, sf::Color(100, 100, 100));

		// Right Border
		std::unique_ptr<Actor> rightBorder = std::make_unique<Actor>(this, m_World.get(), "RightBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, yBorder, rBorderLocation, 0.0f, false, false, true);
		b2ActorInit(rightBorder, sf::Color(100, 100, 100));

#if 1 // debug floor!
		std::unique_ptr<Actor> botBorder = std::make_unique<Actor>(this, m_World.get(), "BotBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, xBorder, dBorderLocation, 0.0f, false, false, true);
		b2ActorInit(botBorder, sf::Color(100, 100, 100));
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
		std::unique_ptr<Actor> boardFrame1 = std::make_unique<Actor>(this, m_World.get(), "board1", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, boardSize, boardPos, 0.0f, false, false, true);
		b2ActorInit(boardFrame1, sf::Color(40, 40, 40, 255));

		const sf::Vector2f netEdgeSize(8.0f, 90.0f);
		const sf::Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);
		
		// Board Frame 2
		std::unique_ptr<Actor> boardFrame2 = std::make_unique<Actor>(this, m_World.get(), "board2", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, netEdgeSize, netEdgePos, 0.0f, false, false, true);
		boardFrame2->GetBodyInstance()->SetTransform(boardFrame2->GetBodyInstance()->GetPosition(), -0.261799388f);
		b2ActorInit(boardFrame2, sf::Color(40, 40, 40, 255));
	
		const sf::Vector2f sensorSize(48.0f, 48.0f);
		const sf::Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);
		
		// Score Sensor
		std::unique_ptr<Actor> scoreSensor = std::make_unique<Actor>(this, m_World.get(), "sensor", EActorShapeType::Circle, Eb2ShapeType::Circle, sensorSize, sensorPos, 0.0f, false, true, true);
		scoreSensor->BindOnBeginoverlap([](Actor* actor) { SensorOverlap(actor); });
		b2ActorInit(scoreSensor, sf::Color(255, 255, 0, 100));

		MakeTrack();
		MakeProjector();
		SetupText();
	}

	return initChecks;
}

void Application::UpdateFrame(const float deltaTime)
{
	LOG_CMD(GetElapsedTime());
	
	// Handle window events
	HandleWindowEvents();
	
	// Update game systems
	UpdateGameSystems();
	
	// Handle input
	HandleInput();
	
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

void Application::HandleInput()
{
	// Handle keyboard input
	HandleKeyboardInput();
	
	// Handle mouse input
	HandleMouseInput();
	
	// Update visual feedback elements
	UpdateVisualFeedback();
}

void Application::HandleKeyboardInput()
{
	// Space bar to start game
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		if (!m_GameState.GetIsGameStarted())
		{
			m_GameState.StartGame();
			m_CenterMessageWidget->m_bIsPaused = false;
		}
	}
}

void Application::HandleMouseInput()
{
	// Left mouse button - charge projection velocity
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
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

	// Right mouse button - spawn ball
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		if (!m_RightMousePressed)
		{
			if (!m_GameState.GetIsGameOver() && m_GameState.GetIsGameStarted())
			{
				SpawnBall();
			}
			m_RightMousePressed = true;
		}
	}
	else
	{
		m_RightMousePressed = false;
	}
	
	// Middle mouse button - reset game
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
	{
		if (!m_MiddleMousePressed)
		{
			m_MiddleMousePressed = true;
			ResetGame();
		}
	}
	else
	{
		m_MiddleMousePressed = false;
	}
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

void Application::ResetGame()
{
	m_GameState.ResetGame();
	ClearTimer();
	m_CenterMessageWidget->Init();
	m_CenterMessageWidget->m_bIsActive = true;
	m_CenterMessageWidget->m_bIsPaused = true;
	m_PivotCache->ResetToInitTransform();
	m_WheelCache->ResetToInitTransform();

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
	// Collapsed function body. Transfering ownership of local unique ptr to the container
	auto setup = [this](std::unique_ptr<Actor>& p, const sf::Color& c) ->void
	{
		p->GetShape()->setOutlineThickness(-1);
		p->GetShape()->setOutlineColor(sf::Color::Black);
		p->GetShape()->setFillColor(c);
		m_B2Actors.push_back(std::move(p));
	};

	// Projector Pivot
	const int row = 14;
	const float viewportX = (float)m_AppWindowData.m_Width;
	const float viewportY = (float)m_AppWindowData.m_Height;
	const sf::Vector2f startLocation(viewportX * 0.15f, viewportY - 16.0f - (row * 32.0f));
	const sf::Vector2f location(startLocation.x + 32.0f, startLocation.y + (row / 2)*32.0f);
	std::unique_ptr<Actor> pivot = std::make_unique<Actor>(this, m_World.get(), "Pivot", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, sf::Vector2f(8.0f, 8.0f), location, 0.0f, false, false, true);
	pivot->BindOnTick([](Actor* actor) { PivotTick(actor); });
	m_PivotCache = pivot.get();
	setup(pivot, sf::Color(0, 0, 255, 100));

	// Projector Wheel
	const sf::Vector2f wheelLocation(location.x - 24, location.y);
	std::unique_ptr<Actor> wheel = std::make_unique<Actor>(this, m_World.get(), "Wheel", EActorShapeType::Circle, Eb2ShapeType::Polygon, sf::Vector2f(48.0f, 48.0f), wheelLocation, 0.0f, false, false, true);
	wheel->GetShape()->setOrigin(wheel->GetShape()->getOrigin() + sf::Vector2f(24, 0));
	wheel->BindOnTick([](Actor* actor) { WheelTick(actor); });
	m_WheelCache = wheel.get();
	setup(wheel, sf::Color(0, 255, 255, 40));
}

void Application::SetupText()
{
	auto pChalkFont = std::shared_ptr<sf::Font>(m_AssetLoader.FindFont(RESOURCES_FONT_CHALK), [](sf::Font*) {});
	auto pPixelFont = std::shared_ptr<sf::Font>(m_AssetLoader.FindFont(RESOURCES_FONT_PIXEL), [](sf::Font*) {});

	const float row1 = 530;
	const float row2 = 620;
	
	// Level Number Widget
	std::unique_ptr<FTextWidget> t1 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(80, row1));
	t1->m_Prefix = "LEVEL\n";
	t1->m_UpdateCallback = [this]() { return m_GameState.GetLevelString(); };
	m_LevelNumberWidget = m_TextProcessor.AddAndInitialize(t1);

	// Score Widget
	std::unique_ptr<FTextWidget> t2 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(80, row2));
	t2->m_Prefix = "SCORE\n";
	t2->m_UpdateCallback = [this]() { return m_GameState.GetScoreString(); };
	m_ScoreWidget = m_TextProcessor.AddAndInitialize(t2);

	// Hi Score Widget
	std::unique_ptr<FTextWidget> t3 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(768, row1));
	t3->m_Prefix = "HISCORE\n";
	t3->m_UpdateCallback = [this]() { return m_GameState.GetHiScoreString(); };
	m_HiScoreWidget = m_TextProcessor.AddAndInitialize(t3);

	// Ball Count Widget
	std::unique_ptr<FTextWidget> t4 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(768, row2));
	t4->m_Prefix = "REQ. BALL\n";
	t4->m_UpdateCallback = [this]() { return m_GameState.GetReqBallString(); };
	m_BallCountWidget = m_TextProcessor.AddAndInitialize(t4);

	// Timer Widget
	std::unique_ptr<FTextWidget> t5 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(368, row1));
	t5->m_Prefix = "REMAINING TIME\n";
	t5->m_Suffix = " S";
	t5->m_UpdateCallback = [this]() { return m_GameState.GetRemainingTimeString(); };
	m_TimerWidget = m_TextProcessor.AddAndInitialize(t5);

	// Elapsed Time Widget
	std::unique_ptr<FTextWidget> t6 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(368, row2));
	t6->m_Prefix = "ELAPSED MIN\n";
	t6->m_Suffix = " S";
	t6->m_UpdateCallback = [this]() { 
		return m_GameState.GetElapsedTimeMinString() + " M" + m_GameState.GetElapsedTimeSecondString(); 
	};
	m_ElapsedTimeWidget = m_TextProcessor.AddAndInitialize(t6);

	// Center Message Widget
	std::unique_ptr<FTextWidget> t7 = std::make_unique<FTextWidget>(pPixelFont, 30, m_AppWindowData.GetViewportCenter());
	t7->m_EndLocation = sf::Vector2f(m_AppWindowData.GetViewportCenter().x, -16);
	t7->m_FadeTime = 1.0f;
	t7->m_RawString = "PRESS SPACE BAR TO START";
	t7->m_bIsPaused = true;
	m_CenterMessageWidget = m_TextProcessor.AddAndInitialize(t7);
}

void Application::SpawnBall()
{
	// Get magnitude of the multiplier.
	const float velocity = m_GameState.GetChargedBallVelocity();

	const sf::Vector2f ballSpawnLocation(m_PivotCache->GetLocation() + sf::Vector2f(32, 32));

	// Construct data to parse.
	FActorSpawnParam spawnParam;
	spawnParam.package = this;
	spawnParam.worldContext = m_World.get();
	spawnParam.name = "Ball";
	spawnParam.shapeType = EActorShapeType::Circle;
	spawnParam.bodyType = Eb2ShapeType::Circle;
	spawnParam.size = sf::Vector2f(32, 32);
	spawnParam.location = ballSpawnLocation;
	spawnParam.rotation = 0.0f;
	spawnParam.isDynamicBody = true;
	spawnParam.generateOverlaps = false;
	spawnParam.autoActivate = true;

	auto findPredicate = [](auto& p)->bool { return !p->IsActive(); };
	auto pActor = std::find_if(m_Balls.begin(), m_Balls.end(), findPredicate);

	// If found
	if (pActor != m_Balls.end())
	{
		Actor* const reuseBall = (*pActor) ? (*pActor).get() : nullptr;
		if (reuseBall)
		{
			reuseBall->GetBodyInstance()->SetLinearVelocity(b2Vec2(velocity, -velocity));
			std::cout << "X: "<<reuseBall->GetBodyInstance()->GetLinearVelocity().x << " Y: "<<reuseBall->GetBodyInstance()->GetLinearVelocity().y << std::endl;
			reuseBall->SetInitTransform(spawnParam.location, spawnParam.rotation);
			reuseBall->ResetToInitTransform();
			reuseBall->Activate();
		}
	}
	else
	{
		// Construct new.
		std::unique_ptr<Actor> ball = std::make_unique<Actor>(spawnParam);
		ball->GetShape()->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_BASKETBALL));
		ball->GetBodyInstance()->SetLinearVelocity(b2Vec2(velocity , -velocity));
		std::cout << "X: " << ball->GetBodyInstance()->GetLinearVelocity().x << " Y: " << ball->GetBodyInstance()->GetLinearVelocity().y << std::endl;
		ball->GetFixtureDefinition()->density = 0.83f;
		ball->GetFixtureDefinition()->friction = 0.4f;
		ball->GetFixtureDefinition()->restitution = 0.65f;
		ball->BindOnTick([](Actor* actor) { BallTick(actor); });
		//ball->BindOnBeginoverlap(BallOverlap);
		m_Balls.push_back(std::move(ball));
	}
}

void Application::PivotTick(Actor* actor)
{
	if (!actor)
	{
		return;
	}

	const float deltaY = 3.0f * cosf(actor->GetPackage()->GetElapsedTime()) / 32.0f;
	b2Vec2 location = actor->GetBodyInstance()->GetPosition() + b2Vec2(0, deltaY);
	actor->GetBodyInstance()->SetTransform(location, actor->GetBodyInstance()->GetAngle());
}

void Application::WheelTick(Actor* actor)
{
	if (!actor)
	{
		return;
	}

	b2Vec2 pivotLocation = actor->GetPackage()->m_PivotCache->GetBodyInstance()->GetPosition();
	actor->GetBodyInstance()->SetTransform(pivotLocation, actor->GetBodyInstance()->GetAngle());
}

void Application::BallTick(Actor* actor)
{
	if (!actor)
	{
		return;
	}

	if (!actor->GetPackage()->m_GameState.GetIsGameStarted())
	{
		return;
	}

	const bool ax = actor->GetLocation().x >= actor->GetPackage()->m_AppWindowData.m_Width + 64.0f;
	const bool bx = actor->GetLocation().x <= -64.0f;
	const bool ay = actor->GetLocation().y >= actor->GetPackage()->m_AppWindowData.m_Height + 64.0f;
	const bool by = actor->GetLocation().y <= -64.0f;

	if (ax || bx || ay || by)
	{
		actor->MakeInactive();
	}
}

void Application::SensorOverlap(Actor* overlapActor)
{
	if (!overlapActor->GetPackage()->m_GameState.GetIsGameStarted())
	{
		return;
	}

	if (overlapActor->GetObjectName() == "Ball")
	{
		overlapActor->GetPackage()->m_GameState.ScoreBall();
	}
}
