#include <SFML/Audio.hpp>

#include "Application.h"
#include "b2Actor2D.h"
#include "b2Actor2DContactListener.h"

Application::Application()
{
	b2ActorContactListner = std::make_unique<b2Actor2DContactListener>();

	Gravity = b2Vec2(0.f, 9.81f);
	World = std::make_shared<b2World>(Gravity);
	World->SetContactListener(b2ActorContactListner.get());
}

Application::~Application() {}

void Application::BeginPlay()
{
	while (m_AppWindow.isOpen())
	{
		m_TimeElapsedSinceLastFrame += m_FixedUpdateClock.restart().asSeconds();
		if (m_TimeElapsedSinceLastFrame >= DELTA_TIME_STEP)
		{
			// Step is used to update physics position/rotation
			World->Step(DELTA_TIME_STEP, 8, 3);

			UpdateFrame(DELTA_TIME_STEP);
			m_TimeElapsedSinceLastFrame -= DELTA_TIME_STEP;

			m_ElapsedTime += DELTA_TIME_STEP;
		}
	}
	EndPlay();
}

int Application::Initialize()
{
	// Reduce the code length, scope in this function only.
	using namespace sf;
	using namespace std;

	bool bInitChecks = true;
	bInitChecks &= GameState.BindApplication(this);
	bInitChecks &= m_AssetLoader.LoadResources();

	if (bInitChecks)
	{
		if ((BGM = m_AssetLoader.FindMusic(RESOURCES_AUDIO_TROLOLO)))
		{
			BGM->setVolume(0);
			BGM->setLooping(true);
			BGM->play();
		}

		// Window creation
		m_AppWindow.create(m_AppWindowData.GetVideoModeFromData(), m_AppWindowData.GetWindowName());
		m_AppWindow.setVerticalSyncEnabled(true);
		//AppWindow.setActive();

		// Border creations
		const float BorderThickness = 16.0f;
		const float viewportX = (float)m_AppWindowData.Width;
		const float viewportY = (float)m_AppWindowData.Height;
		const Vector2f XBorder(viewportX, BorderThickness);
		const Vector2f YBorder(BorderThickness, viewportY * 0.7f);
		const Vector2f UBorderLocation(viewportX * 0.5f						, BorderThickness * 0.5f);
		const Vector2f DBorderLocation(viewportX * 0.5f						, viewportY - BorderThickness * 0.5f);
		const Vector2f LBorderLocation(BorderThickness * 0.5f				, viewportY * 0.5f - (viewportY * .15f) ); // 1 - .7f div 2
		const Vector2f RBorderLocation(viewportX - BorderThickness * 0.5f	, viewportY * 0.5f - (viewportY * .15f) ); // 1 - .7f div 2

		// Collapsed function body. Transfering ownership of local unique ptr to the container
		auto b2ActorInit = [this](unique_ptr<b2Actor2D>& p, const Color c) ->void 
		{
			p->GetShape()->setOutlineThickness(-1);
			p->GetShape()->setOutlineColor(Color::Black);
			p->GetShape()->setFillColor(c);
			b2Actors.push_back(move(p));
		};

		unique_ptr<b2Actor2D> TopBorder = make_unique<b2Actor2D>(this, World.get(), "TopBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, UBorderLocation);
		b2ActorInit(TopBorder, Color(100, 100, 100));

		unique_ptr<b2Actor2D> LeftBorder = make_unique<b2Actor2D>(this, World.get(), "LeftBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, LBorderLocation);
		b2ActorInit(LeftBorder , Color(100, 100, 100) );

		unique_ptr<b2Actor2D> RightBorder = make_unique<b2Actor2D>(this, World.get(), "RightBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, RBorderLocation);
		b2ActorInit(RightBorder,Color(100, 100, 100));

#if 1 // debug floor!
		unique_ptr<b2Actor2D> BotBorder = make_unique<b2Actor2D>(this, World.get(), "BotBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, DBorderLocation);
		b2ActorInit(BotBorder, Color(100, 100, 100));
#endif 
		unique_ptr<RectangleShape> Background = make_unique<RectangleShape>(Vector2f(viewportX, viewportY));
		Background->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_BACKGROUND));
		RenderShapes.push_back(move(Background));

		unique_ptr<RectangleShape> Scoreboard = make_unique<RectangleShape>(Vector2f(viewportX, viewportY * .3f));
		Scoreboard->setPosition({0.0f, viewportY * 0.7f});
		Scoreboard->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_CHALKBOARD));
		RenderShapes.push_back(move(Scoreboard));

		unique_ptr<RectangleShape> ChargeGaugeMaxUniquePtr = make_unique<RectangleShape>();
		ChargeGaugeMaxUniquePtr->setFillColor(Color(145, 145, 145, 255));
		ChargeGaugeMaxUniquePtr->setSize(Vector2f(160.0f, 8.0f));
		ChargeGaugeMax = ChargeGaugeMaxUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		RenderShapes.push_back(move(ChargeGaugeMaxUniquePtr));

		unique_ptr<RectangleShape> ChargeGaugeProgressUniquePtr = make_unique<RectangleShape>();
		ChargeGaugeProgressUniquePtr->setFillColor(Color::Yellow);
		ChargeGaugeProgressUniquePtr->setFillColor(Color::Yellow);
		ChargeGaugeProgress = ChargeGaugeProgressUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		RenderShapes.push_back(move(ChargeGaugeProgressUniquePtr));

		for (int i = 0; i < 2; i++)
		{
			AngleIndicators[i].color = (i == 1) ? Color::Cyan : Color::Blue;
		}
		
		// Board
		const float offsetX = viewportX * 0.98f;
		const float offsetY = viewportY * 0.35f;
		const Vector2f boardSize(8.0f, 200.0f);
		const Vector2f boardPos(viewportX * 0.98f, viewportY * 0.35f);

		unique_ptr<b2Actor2D> BoardFrame1 = make_unique<b2Actor2D>(this, World.get(), "board1", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, boardSize, boardPos);
		b2ActorInit(BoardFrame1, Color(40, 40, 40, 255));

		const Vector2f netEdgeSize(8.0f, 90.0f);
		const Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);
		
		unique_ptr<b2Actor2D> BoardFrame2 = make_unique<b2Actor2D>(this, World.get(), "board2", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, netEdgeSize, netEdgePos);
		BoardFrame2->GetBodyInstance()->SetTransform(BoardFrame2->GetBodyInstance()->GetPosition(), -0.261799388f);
		b2ActorInit(BoardFrame2, Color(40, 40, 40, 255));
	
		const Vector2f sensorSize(48.0f, 48.0f);
		const Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);
		
		unique_ptr<b2Actor2D> ScoreSensor = make_unique<b2Actor2D>(this, World.get(), "sensor", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, sensorSize, sensorPos, 0.0f, false, true);
		ScoreSensor->BindOnBeginoverlap(SensorOverlap);
		b2ActorInit(ScoreSensor, Color(255, 255, 0, 100));

		MakeTrack();
		MakeProjector();
		SetupText();
	}

	return bInitChecks;
}

void Application::UpdateFrame(const float DeltaTime)
{
	std::optional<sf::Event> event = m_AppWindow.pollEvent();
	if (event.has_value() && event.value().is<sf::Event::Closed>())
	{
		m_AppWindow.close();
	}

	GameState.Tick();
	m_TextProcessor.Tick();

	for (auto&& element : b2Actors)
	{
		if (element)
		{
			element->Tick();
		}
	}
		

	//for (auto& element : Balls)
	//{
	//	if (element)
	//	{
	//		element->Tick();
	//	}
	//}
	
	// Need to update on tick.
	m_LevelNumberWidget->m_Text->setString("LEVEL\n" + GameState.GetLevelString());
	m_ScoreWidget->m_Text->setString("SCORE\n" + GameState.GetScoreString());
	m_HiScoreWidget->m_Text->setString("HISCORE\n" + GameState.GetHiScoreString());
	m_BallCountWidget->m_Text->setString("REQ. BALL\n" + GameState.GetReqBallString());
	m_TimerWidget->m_Text->setString("REMAINING TIME\n" + GameState.GetRemainingTimeString() + " S");
	m_ElapsedTimeWidget->m_Text->setString("ELAPSED MIN\n" + GameState.GetElapsedTimeMinString() + " M" + GameState.GetElapsedTimeSecondString() + " S");

#if 0
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		if (!GameState.GetIsGameStarted())
		{
			GameState.StartGame();
			m_CenterMessageWidget->m_bIsPaused = false;
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{

		//If the game already Started, Do something else.
		if (!GameState.GetIsGameOver() && GameState.GetIsGameStarted())
		{
			GameState.ChargeProjectionVelocity();
		}
	}
	else
	{
		GameState.DischargeProjectionVelocity();
	}

	// Right Click to Spawn Ball.
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		if (!bRightMousePressed)
		{
			if (!GameState.GetIsGameOver() && GameState.GetIsGameStarted())
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

			GameState.ResetGame();
			ClearTimer();
			m_CenterMessageWidget->Init();
			m_CenterMessageWidget->m_bIsActive = true;
			m_CenterMessageWidget->m_bIsPaused = true;
			PivotCache->ResetToInitTransform();
			WheelCache->ResetToInitTransform();

			for (auto& i : Balls)
				i->MakeInactive();
		}
	}
	else
	{
		bMiddleMousePressed = false;
	}
#endif

	// Update Info Gauge
	float maxVelocity = 60.0f;
	float percentage = GameState.GetChargedBallVelocity() / maxVelocity;

	const sf::Vector2f PivotLocation = PivotCache->GetLocation();
	const sf::Vector2f MouseLocation = sf::Vector2f(sf::Mouse::getPosition(m_AppWindow));
	const sf::Vector2f OffsetMouseLocation = sf::Vector2f(sf::Vector2i(sf::Mouse::getPosition(m_AppWindow)) - sf::Vector2i(16, 16));

	ChargeGaugeMax->setPosition(OffsetMouseLocation);
	ChargeGaugeMax->setSize({160.0f, 8.0f});
	ChargeGaugeProgress->setPosition(OffsetMouseLocation);
	ChargeGaugeProgress->setSize(sf::Vector2f(160.0f * percentage, 8.0f));;

	// Update Angle Indicator
	AngleIndicators[0].position = PivotLocation;
	AngleIndicators[1].position = MouseLocation;

	// Rendering
	m_AppWindow.clear(CORNFLOWER_BLUE);

	for (auto& Itr : RenderShapes)
		m_AppWindow.draw(*Itr);


	for (auto& Itr : b2Actors)
		m_AppWindow.draw(*Itr->GetShape());

	for (auto& Itr : Balls)
	{
		m_AppWindow.draw(*Itr->GetShape());
		m_AppWindow.draw(*Itr->DebugForward);
	}

	for (auto& Itr : m_TextProcessor.GetTextWidgets())
	{
		if(Itr->m_bIsActive)
			m_AppWindow.draw(*Itr->m_Text);
	}

	m_AppWindow.draw(AngleIndicators, 2, sf::PrimitiveType::Lines);
	m_AppWindow.display();
}

void Application::EndPlay()
{
}

void Application::MakeTrack()
{
	using namespace std;
	using namespace sf;

	// The Track 
	const float ViewportX = (float)m_AppWindowData.Width;
	const float ViewportY = (float)m_AppWindowData.Height;
	const int Row = 14;
	const int Column = 2;

	const Vector2f StartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row*32.0f));
	const Vector2f Size(32.0f, 32.0f);

	unique_ptr<RectangleShape> Tracks[Row][Column];
	for (int i = 0; i < Row; ++i)
	{
		for (int j = 0; j < Column; ++j)
		{
			Tracks[i][j] = make_unique<RectangleShape>();

			Tracks[i][j]->setSize(Size);
			Tracks[i][j]->setPosition({StartLocation.x + j * 32.0f, StartLocation.y + i * 32.0f});
			Tracks[i][j]->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_BOXALT));
			RenderShapes.push_back(move(Tracks[i][j]));
		}
	}
}

void Application::MakeProjector()
{
	using namespace std;
	using namespace sf;

	// Collapsed function body. Transfering ownership of local unique ptr to the container
	auto Setup = [this](unique_ptr<b2Actor2D>& p, const Color c) ->void
	{
		p->GetShape()->setOutlineThickness(-1);
		p->GetShape()->setOutlineColor(Color::Black);
		p->GetShape()->setFillColor(c);
		b2Actors.push_back(move(p));
	};

	// Projector Pivot
	const int Row = 14;
	const float ViewportX = (float)m_AppWindowData.Width;
	const float ViewportY = (float)m_AppWindowData.Height;
	const Vector2f StartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row * 32.0f));
	const Vector2f Location(StartLocation.x + 32.0f, StartLocation.y + (Row / 2)*32.0f);
	unique_ptr<b2Actor2D> Pivot = make_unique<b2Actor2D>(this, World.get(), "Pivot", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, Vector2f(8.0f, 8.0f), Location, 0.0f, false, false);
	Pivot->BindOnTick(PivotTick);
	PivotCache = Pivot.get();
	Setup(Pivot, Color(0, 0, 255, 100));

	// Projector Wheel
	const Vector2f WheelLocation(Location.x - 24, Location.y);
	unique_ptr<b2Actor2D> Wheel = make_unique<b2Actor2D>(this, World.get(), "Wheel", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Polygon, Vector2f(48.0f, 48.0f), WheelLocation, 0.0f, false, false);
	Wheel->GetShape()->setOrigin(Wheel->GetShape()->getOrigin() + Vector2f(24, 0));
	Wheel->BindOnTick(WheelTick);
	WheelCache = Wheel.get();
	Setup(Wheel, Color(0, 255, 255, 40));
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
	const float velocity = GameState.GetChargedBallVelocity();

	const sf::Vector2f BallSpawnLocation(PivotCache->GetLocation() + sf::Vector2f(32, 32));

	// Construct data to parse.
	Fb2ActorSpawnParam SpawnParam;
	SpawnParam.Package = this;
	SpawnParam.WorldContext = World.get();
	SpawnParam.Name = "Ball";
	SpawnParam.ShapeType = EActorShapeType::EST_Circle;
	SpawnParam.BodyType = Eb2ShapeType::ECT_Circle;
	SpawnParam.Size = sf::Vector2f(32, 32);
	SpawnParam.Location = BallSpawnLocation;
	SpawnParam.Rotation = 0.0f;
	SpawnParam.bIsDynamicBody = true;
	SpawnParam.bGenerateOverlaps = false;
	SpawnParam.bAutoActivate = true;

	auto FindPredicate = [](auto& P)->bool { return !P->IsActive(); };
	auto pActor = std::find_if(Balls.begin(), Balls.end(), FindPredicate);

	// If found
	if (pActor != Balls.end())
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
		Balls.push_back(std::move(Ball));
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

	b2Vec2 PivotLocation = Actor->GetPackage()->PivotCache->GetBodyInstance()->GetPosition();
	Actor->GetBodyInstance()->SetTransform(PivotLocation, Actor->GetBodyInstance()->GetAngle());
}

void Application::BallTick(b2Actor2D* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (!Actor->GetPackage()->GameState.GetIsGameStarted())
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
	if (!OverlapActor->GetPackage()->GameState.GetIsGameStarted())
	{
		return;
	}

	if (OverlapActor->GetObjectName() == "Ball")
	{
		OverlapActor->GetPackage()->GameState.ScoreBall();
	}
}
