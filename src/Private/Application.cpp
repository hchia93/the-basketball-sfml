#include <SFML/Audio.hpp>

#include "Application.h"
#include "b2Actor2D.h"
#include "b2Actor2DContactListener.h"

Application::Application()
{
	b2ActorContactListner = std::make_unique<b2Actor2DContactListener>();

	Gravity = b2Vec2(0.f, 9.81f);
	m_World = std::make_shared<b2World>(Gravity);
	m_World->SetContactListener(b2ActorContactListner.get());
}

Application::~Application()
{

}

void Application::BeginPlay()
{
	while (m_AppWindow.isOpen())
	{
		m_TimeElapsedSinceLastFrame += m_FixedUpdateClock.restart().asSeconds();
		if (m_TimeElapsedSinceLastFrame >= DELTA_TIME_STEP)
		{
			// Step is used to update physics position/rotation
			m_World->Step(DELTA_TIME_STEP, 8, 3);

			UpdateFrame(DELTA_TIME_STEP);
			m_TimeElapsedSinceLastFrame -= DELTA_TIME_STEP;

			m_ElapsedTime += DELTA_TIME_STEP;
		}
		
		// Add frame limiting to prevent excessive CPU usage
		sf::sleep(sf::milliseconds(1));
	}
}

int Application::Initialize()
{
	// Reduce the code length, scope in this function only.

	bool bInitChecks = true;
	bInitChecks &= m_GameState.BindApplication(this);
	bInitChecks &= m_AssetLoader.LoadResources();

	if (bInitChecks)
	{
		if ((m_BGM = m_AssetLoader.FindMusic(RESOURCES_AUDIO_TROLOLO)))
		{
			m_BGM->setVolume(0);
			m_BGM->setLooping(true);
			m_BGM->play();
		}

		// Window creation
		m_AppWindow.create(m_AppWindowData.GetVideoModeFromData(), m_AppWindowData.GetWindowName());
		m_AppWindow.setVerticalSyncEnabled(true);
		//AppWindow.setActive();

		// Border creations
		const float BorderThickness = 16.0f;
		const float viewportX = (float)m_AppWindowData.Width;
		const float viewportY = (float)m_AppWindowData.Height;
		const sf::Vector2f XBorder(viewportX, BorderThickness);
		const sf::Vector2f YBorder(BorderThickness, viewportY * 0.7f);
		const sf::Vector2f UBorderLocation(viewportX * 0.5f						, BorderThickness * 0.5f);
		const sf::Vector2f DBorderLocation(viewportX * 0.5f						, viewportY - BorderThickness * 0.5f);
		const sf::Vector2f LBorderLocation(BorderThickness * 0.5f				, viewportY * 0.5f - (viewportY * .15f) ); // 1 - .7f div 2
		const sf::Vector2f RBorderLocation(viewportX - BorderThickness * 0.5f	, viewportY * 0.5f - (viewportY * .15f) ); // 1 - .7f div 2

		// Collapsed function body. Transfering ownership of local unique ptr to the container
		auto b2ActorInit = [this](std::unique_ptr<b2Actor2D>& actor, const sf::Color color) ->void 
		{
			actor->GetShape()->setOutlineThickness(-1);
			actor->GetShape()->setOutlineColor(sf::Color::Black);
			actor->GetShape()->setFillColor(color);
			b2Actors.push_back(std::move(actor));
		};

		// Top Border
		std::unique_ptr<b2Actor2D> TopBorder = std::make_unique<b2Actor2D>(this, m_World.get(), "TopBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, XBorder, UBorderLocation, 0.0f, false, false, true);
		b2ActorInit(TopBorder, sf::Color(100, 100, 100));

		// Left Border
		std::unique_ptr<b2Actor2D> LeftBorder = std::make_unique<b2Actor2D>(this, m_World.get(), "LeftBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, YBorder, LBorderLocation, 0.0f, false, false, true);
		b2ActorInit(LeftBorder, sf::Color(100, 100, 100));

		// Right Border
		std::unique_ptr<b2Actor2D> RightBorder = std::make_unique<b2Actor2D>(this, m_World.get(), "RightBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, YBorder, RBorderLocation, 0.0f, false, false, true);
		b2ActorInit(RightBorder, sf::Color(100, 100, 100));

#if 1 // debug floor!
		std::unique_ptr<b2Actor2D> BotBorder = std::make_unique<b2Actor2D>(this, m_World.get(), "BotBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, XBorder, DBorderLocation, 0.0f, false, false, true);
		b2ActorInit(BotBorder, sf::Color(100, 100, 100));
#endif 
		std::unique_ptr<sf::RectangleShape> Background = std::make_unique<sf::RectangleShape>(sf::Vector2f(viewportX, viewportY));
		Background->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_BACKGROUND));
		RenderShapes.push_back(std::move(Background));

		std::unique_ptr<sf::RectangleShape> Scoreboard = std::make_unique<sf::RectangleShape>(sf::Vector2f(viewportX, viewportY * .3f));
		Scoreboard->setPosition({0.0f, viewportY * 0.7f});
		Scoreboard->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_CHALKBOARD));
		RenderShapes.push_back(std::move(Scoreboard));

		std::unique_ptr<sf::RectangleShape> ChargeGaugeMaxUniquePtr = std::make_unique<sf::RectangleShape>();
		ChargeGaugeMaxUniquePtr->setFillColor(sf::Color(145, 145, 145, 255));
		ChargeGaugeMaxUniquePtr->setSize(sf::Vector2f(160.0f, 8.0f));
		ChargeGaugeMax = ChargeGaugeMaxUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		RenderShapes.push_back(std::move(ChargeGaugeMaxUniquePtr));

		std::unique_ptr<sf::RectangleShape> ChargeGaugeProgressUniquePtr = std::make_unique<sf::RectangleShape>();
		ChargeGaugeProgressUniquePtr->setFillColor(sf::Color::Yellow);
		ChargeGaugeProgressUniquePtr->setFillColor(sf::Color::Yellow);
		ChargeGaugeProgress = ChargeGaugeProgressUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		RenderShapes.push_back(std::move(ChargeGaugeProgressUniquePtr));

		for (int i = 0; i < 2; i++)
		{
			AngleIndicators[i].color = (i == 1) ? sf::Color::Cyan : sf::Color::Blue;
		}
		
		// Board
		const float offsetX = viewportX * 0.98f;
		const float offsetY = viewportY * 0.35f;
		const sf::Vector2f boardSize(8.0f, 200.0f);
		const sf::Vector2f boardPos(viewportX * 0.98f, viewportY * 0.35f);

		// Board Frame 1
		std::unique_ptr<b2Actor2D> BoardFrame1 = std::make_unique<b2Actor2D>(this, m_World.get(), "board1", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, boardSize, boardPos, 0.0f, false, false, true);
		b2ActorInit(BoardFrame1, sf::Color(40, 40, 40, 255));

		const sf::Vector2f netEdgeSize(8.0f, 90.0f);
		const sf::Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);
		
		// Board Frame 2
		std::unique_ptr<b2Actor2D> BoardFrame2 = std::make_unique<b2Actor2D>(this, m_World.get(), "board2", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, netEdgeSize, netEdgePos, 0.0f, false, false, true);
		BoardFrame2->GetBodyInstance()->SetTransform(BoardFrame2->GetBodyInstance()->GetPosition(), -0.261799388f);
		b2ActorInit(BoardFrame2, sf::Color(40, 40, 40, 255));
	
		const sf::Vector2f sensorSize(48.0f, 48.0f);
		const sf::Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);
		
		// Score Sensor
		std::unique_ptr<b2Actor2D> ScoreSensor = std::make_unique<b2Actor2D>(this, m_World.get(), "sensor", EActorShapeType::Circle, Eb2ShapeType::Circle, sensorSize, sensorPos, 0.0f, false, true, true);
		ScoreSensor->BindOnBeginoverlap(SensorOverlap);
		b2ActorInit(ScoreSensor, sf::Color(255, 255, 0, 100));

		MakeTrack();
		MakeProjector();
		SetupText();
	}

	return bInitChecks;
}

void Application::UpdateFrame(const float DeltaTime)
{
	LOG_CMD(GetElapsedTime());
	std::optional<sf::Event> event = m_AppWindow.pollEvent();
	if (event.has_value() && event.value().is<sf::Event::Closed>())
	{
		m_AppWindow.close();
	}

	m_GameState.Tick();
	m_TextProcessor.Tick();

	for (auto&& element : b2Actors)
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
	
	// Need to update on tick.
	m_LevelNumberWidget->m_Text->setString("LEVEL\n" + m_GameState.GetLevelString());
	m_ScoreWidget->m_Text->setString("SCORE\n" + m_GameState.GetScoreString());
	m_HiScoreWidget->m_Text->setString("HISCORE\n" + m_GameState.GetHiScoreString());
	m_BallCountWidget->m_Text->setString("REQ. BALL\n" + m_GameState.GetReqBallString());
	m_TimerWidget->m_Text->setString("REMAINING TIME\n" + m_GameState.GetRemainingTimeString() + " S");
	m_ElapsedTimeWidget->m_Text->setString("ELAPSED MIN\n" + m_GameState.GetElapsedTimeMinString() + " M" + m_GameState.GetElapsedTimeSecondString() + " S");


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		if (!m_GameState.GetIsGameStarted())
		{
			m_GameState.StartGame();
			m_CenterMessageWidget->m_bIsPaused = false;
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{

		//If the game already Started, Do something else.
		if (!m_GameState.GetIsGameOver() && m_GameState.GetIsGameStarted())
		{
			m_GameState.ChargeProjectionVelocity();
		}
	}
	else
	{
		m_GameState.DischargeProjectionVelocity();
	}

	// Right Click to Spawn Ball.
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		if (!bRightMousePressed)
		{
			if (!m_GameState.GetIsGameOver() && m_GameState.GetIsGameStarted())
			{
				SpawnBall();
			}
			bRightMousePressed = true;
		}
	}
	else
	{
		bRightMousePressed = false;
	}
	// Middle Button ： Reset
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
	{
		if (!bMiddleMousePressed)
		{
			bMiddleMousePressed = true;

			m_GameState.ResetGame();
			ClearTimer();
			m_CenterMessageWidget->Init();
			m_CenterMessageWidget->m_bIsActive = true;
			m_CenterMessageWidget->m_bIsPaused = true;
			pivotCache->ResetToInitTransform();
			WheelCache->ResetToInitTransform();

			for (auto& i : m_Balls)
				i->MakeInactive();
		}
	}
	else
	{
		bMiddleMousePressed = false;
	}

	// Update Info Gauge
	float maxVelocity = 60.0f;
	float percentage = m_GameState.GetChargedBallVelocity() / maxVelocity;

	sf::Vector2i mouseLocation = sf::Mouse::getPosition(m_AppWindow);
	const sf::Vector2f offsetMouseLocation = sf::Vector2f(mouseLocation - sf::Vector2i(16, 16));

	ChargeGaugeMax->setPosition(offsetMouseLocation);
	ChargeGaugeMax->setSize({160.0f, 8.0f});
	ChargeGaugeProgress->setPosition(offsetMouseLocation);
	ChargeGaugeProgress->setSize(sf::Vector2f(160.0f * percentage, 8.0f));;

	// Update Angle Indicator
	AngleIndicators[0].position = pivotCache->GetLocation();
	AngleIndicators[1].position = sf::Vector2f(mouseLocation);

	// Rendering
	m_AppWindow.clear(CORNFLOWER_BLUE);

	for (auto& element : RenderShapes)
	{
		if (element)
		{
			m_AppWindow.draw(*element);
		}
	}

	for (auto& element : b2Actors)
	{
		if (element)
		{
			m_AppWindow.draw(*element->GetShape());
		}
	}

	for (auto& element : m_Balls)
	{
		if (element)
		{
			m_AppWindow.draw(*element->GetShape());
			m_AppWindow.draw(*element->DebugForward);
		}
	}

	for (auto& widget : m_TextProcessor.GetTextWidgets())
	{
		if(widget && widget->m_bIsActive)
		{
			m_AppWindow.draw(*widget->m_Text);
		}
	}

	m_AppWindow.draw(AngleIndicators, 2, sf::PrimitiveType::Lines);
	m_AppWindow.display();
}


void Application::MakeTrack()
{
	// The Track 
	const float ViewportX = (float)m_AppWindowData.Width;
	const float ViewportY = (float)m_AppWindowData.Height;
	const int Row = 14;
	const int Column = 2;

	const sf::Vector2f StartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row*32.0f));
	const sf::Vector2f Size(32.0f, 32.0f);

	std::unique_ptr<sf::RectangleShape> Tracks[Row][Column];
	for (int i = 0; i < Row; ++i)
	{
		for (int j = 0; j < Column; ++j)
		{
			Tracks[i][j] = std::make_unique<sf::RectangleShape>();

			Tracks[i][j]->setSize(Size);
			Tracks[i][j]->setPosition({StartLocation.x + j * 32.0f, StartLocation.y + i * 32.0f});
			Tracks[i][j]->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_BOXALT));
			RenderShapes.push_back(std::move(Tracks[i][j]));
		}
	}
}

void Application::MakeProjector()
{
	// Collapsed function body. Transfering ownership of local unique ptr to the container
	auto Setup = [this](std::unique_ptr<b2Actor2D>& p, const sf::Color c) ->void
	{
		p->GetShape()->setOutlineThickness(-1);
		p->GetShape()->setOutlineColor(sf::Color::Black);
		p->GetShape()->setFillColor(c);
		b2Actors.push_back(std::move(p));
	};

	// Projector Pivot
	const int Row = 14;
	const float ViewportX = (float)m_AppWindowData.Width;
	const float ViewportY = (float)m_AppWindowData.Height;
	const sf::Vector2f StartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row * 32.0f));
	const sf::Vector2f Location(StartLocation.x + 32.0f, StartLocation.y + (Row / 2)*32.0f);
	std::unique_ptr<b2Actor2D> Pivot = std::make_unique<b2Actor2D>(this, m_World.get(), "Pivot", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, sf::Vector2f(8.0f, 8.0f), Location, 0.0f, false, false, true);
	Pivot->BindOnTick(PivotTick);
	pivotCache = Pivot.get();
	Setup(Pivot, sf::Color(0, 0, 255, 100));

	// Projector Wheel
	const sf::Vector2f WheelLocation(Location.x - 24, Location.y);
	std::unique_ptr<b2Actor2D> Wheel = std::make_unique<b2Actor2D>(this, m_World.get(), "Wheel", EActorShapeType::Circle, Eb2ShapeType::Polygon, sf::Vector2f(48.0f, 48.0f), WheelLocation, 0.0f, false, false, true);
	Wheel->GetShape()->setOrigin(Wheel->GetShape()->getOrigin() + sf::Vector2f(24, 0));
	Wheel->BindOnTick(WheelTick);
	WheelCache = Wheel.get();
	Setup(Wheel, sf::Color(0, 255, 255, 40));
}

void Application::SetupText()
{
	auto pChalkFont = std::shared_ptr<sf::Font>(m_AssetLoader.FindFont(RESOURCES_FONT_CHALK), [](sf::Font*) {});
	auto pPixelFont = std::shared_ptr<sf::Font>(m_AssetLoader.FindFont(RESOURCES_FONT_PIXEL), [](sf::Font*) {});

	const float row1 = 530;
	const float row2 = 620;
	
	std::unique_ptr<FTextWidget> t1 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(80, row1));
	m_LevelNumberWidget = m_TextProcessor.AddAndInitialize(t1);

	std::unique_ptr<FTextWidget> t2 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(80, row2));
	m_ScoreWidget = m_TextProcessor.AddAndInitialize(t2);

	std::unique_ptr<FTextWidget> t3 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(768, row1));
	m_HiScoreWidget = m_TextProcessor.AddAndInitialize(t3);

	std::unique_ptr<FTextWidget> t4 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(768, row2));
	m_BallCountWidget = m_TextProcessor.AddAndInitialize(t4);

	std::unique_ptr<FTextWidget> t5 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(368, row1));
	m_TimerWidget = m_TextProcessor.AddAndInitialize(t5);

	std::unique_ptr<FTextWidget> t6 = std::make_unique<FTextWidget>(pChalkFont, 30, sf::Vector2f(368, row2));
	m_ElapsedTimeWidget = m_TextProcessor.AddAndInitialize(t6);

	std::unique_ptr<FTextWidget> t7 = std::make_unique<FTextWidget>(pPixelFont, 30, m_AppWindowData.GetViewportCenter()); // middle
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

	const sf::Vector2f BallSpawnLocation(pivotCache->GetLocation() + sf::Vector2f(32, 32));

	// Construct data to parse.
	Fb2ActorSpawnParam SpawnParam;
	SpawnParam.Package = this;
	SpawnParam.WorldContext = m_World.get();
	SpawnParam.Name = "Ball";
	SpawnParam.ShapeType = EActorShapeType::Circle;
	SpawnParam.BodyType = Eb2ShapeType::Circle;
	SpawnParam.Size = sf::Vector2f(32, 32);
	SpawnParam.Location = BallSpawnLocation;
	SpawnParam.Rotation = 0.0f;
	SpawnParam.bIsDynamicBody = true;
	SpawnParam.bGenerateOverlaps = false;
	SpawnParam.bAutoActivate = true;

	auto FindPredicate = [](auto& P)->bool { return !P->IsActive(); };
	auto pActor = std::find_if(m_Balls.begin(), m_Balls.end(), FindPredicate);

	// If found
	if (pActor != m_Balls.end())
	{
		b2Actor2D* const ReuseBall = (*pActor) ? (*pActor).get() : nullptr;
		if (ReuseBall)
		{
			ReuseBall->GetBodyInstance()->SetLinearVelocity(b2Vec2(velocity, -velocity));
			std::cout << "X: "<<ReuseBall->GetBodyInstance()->GetLinearVelocity().x << " Y: "<<ReuseBall->GetBodyInstance()->GetLinearVelocity().y << std::endl;
			ReuseBall->SetInitTransform(SpawnParam.Location, SpawnParam.Rotation);
			ReuseBall->ResetToInitTransform();
			ReuseBall->Activate();
		}
	}
	else
	{
		// Construct new.
		std::unique_ptr<b2Actor2D> Ball = std::make_unique<b2Actor2D>(SpawnParam);
		Ball->GetShape()->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_BASKETBALL));
		Ball->GetBodyInstance()->SetLinearVelocity(b2Vec2(velocity , -velocity));
		std::cout << "X: " << Ball->GetBodyInstance()->GetLinearVelocity().x << " Y: " << Ball->GetBodyInstance()->GetLinearVelocity().y << std::endl;
		Ball->GetFixtureDefinition()->density = 0.83f;
		Ball->GetFixtureDefinition()->friction = 0.4f;
		Ball->GetFixtureDefinition()->restitution = 0.65f;
		Ball->BindOnTick(BallTick);
		//Ball->BindOnBeginoverlap(BallOverlap);
		m_Balls.push_back(std::move(Ball));
	}
}

void Application::PivotTick(b2Actor2D* Actor)
{
	if (!Actor)
	{
		return;
	}

	const float deltaY = 3.0f * cosf(Actor->GetPackage()->GetElapsedTime()) / 32.0f;
	b2Vec2 Location = Actor->GetBodyInstance()->GetPosition() + b2Vec2(0, deltaY);
	Actor->GetBodyInstance()->SetTransform(Location, Actor->GetBodyInstance()->GetAngle());
}

void Application::WheelTick(b2Actor2D* Actor)
{
	if (!Actor)
	{
		return;
	}

	b2Vec2 pivotLocation = Actor->GetPackage()->pivotCache->GetBodyInstance()->GetPosition();
	Actor->GetBodyInstance()->SetTransform(pivotLocation, Actor->GetBodyInstance()->GetAngle());
}

void Application::BallTick(b2Actor2D* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (!Actor->GetPackage()->m_GameState.GetIsGameStarted())
	{
		return;
	}

	const bool Ax = Actor->GetLocation().x >= Actor->GetPackage()->m_AppWindowData.Width + 64.0f;
	const bool Bx = Actor->GetLocation().x <= -64.0f;
	const bool Ay = Actor->GetLocation().y >= Actor->GetPackage()->m_AppWindowData.Height + 64.0f;
	const bool By = Actor->GetLocation().y <= -64.0f;

	if (Ax || Bx || Ay || By)
	{
		Actor->MakeInactive();
	}
}

void Application::SensorOverlap(b2Actor2D* OverlapActor)
{
	if (!OverlapActor->GetPackage()->m_GameState.GetIsGameStarted())
	{
		return;
	}

	if (OverlapActor->GetObjectName() == "Ball")
	{
		OverlapActor->GetPackage()->m_GameState.ScoreBall();
	}
}
