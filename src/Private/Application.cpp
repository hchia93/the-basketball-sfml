#include <SFML/Audio.hpp>

#include "Application.h"
#include "b2Actor2D.h"
#include "b2Actor2DContactListener.h"

Application::Application()
{
	m_b2ActorContactListner = std::make_unique<b2Actor2DContactListener>();

	m_Gravity = b2Vec2(0.0f, 9.81f);
	m_WorldId = Box2DHelper::CreateWorld(m_Gravity);
}

Application::~Application() 
{
	if (Box2DHelper::IsValid(m_WorldId))
	{
		Box2DHelper::DestroyWorld(m_WorldId);
	}
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
	// Reduce the code length, scope in this function only.
	using namespace sf;
	using namespace std;

	bool bInitChecks = true;
	bInitChecks &= m_TickHandle.BindApplication(this);
	bInitChecks &= m_GameState.BindApplication(this);
	bInitChecks &= m_TextRenderer.BindApplication(this);
	bInitChecks &= m_AssetLoader.LoadResources();

	if (bInitChecks)
	{
		if (m_BGM = FAssetLoader::FindMusic(&m_AssetLoader, RESOURCES_AUDIO_TROLOLO))
		{
			m_BGM->setVolume(0);
			m_BGM->play();
		}

		// Window creation
		m_AppWindow.create(m_RenderWindowData.GetVideoModeFromData(), m_RenderWindowData.m_WindowName);
		m_AppWindow.setVerticalSyncEnabled(true);
		m_AppWindow.setActive();

		// Border creations
		const float BorderThickness = 16.0f;
		const float ViewportX = (float)m_RenderWindowData.m_WindowSize.x;
		const float ViewportY = (float)m_RenderWindowData.m_WindowSize.y;
		const Vector2f XBorder(ViewportX, BorderThickness);
		const Vector2f YBorder(BorderThickness, ViewportY * 0.7f);
		const Vector2f UBorderLocation(ViewportX * 0.5f, BorderThickness * 0.5f);
		const Vector2f DBorderLocation(ViewportX * 0.5f, ViewportY - BorderThickness * 0.5f);
		const Vector2f LBorderLocation(BorderThickness * 0.5f, ViewportY * 0.5f - (ViewportY * 0.15f)); // 1 - .7f div 2
		const Vector2f RBorderLocation(ViewportX - BorderThickness * 0.5f, ViewportY * 0.5f - (ViewportY * 0.15f)); // 1 - .7f div 2

		// Collapsed function body. Transfering ownership of local unique ptr to the container
		auto b2ActorInit = [this](unique_ptr<b2Actor2D>& p, const Color c) -> void 
		{
			p->GetShape()->setOutlineThickness(-1);
			p->GetShape()->setOutlineColor(Color::Black);
			p->GetShape()->setFillColor(c);
			m_b2Actors.push_back(move(p));
		};

		unique_ptr<b2Actor2D> TopBorder = make_unique<b2Actor2D>(this, m_WorldId, "TopBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, XBorder, UBorderLocation);
		b2ActorInit(TopBorder, Color(100, 100, 100));

		unique_ptr<b2Actor2D> LeftBorder = make_unique<b2Actor2D>(this, m_WorldId, "LeftBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, YBorder, LBorderLocation);
		b2ActorInit(LeftBorder, Color(100, 100, 100));

		unique_ptr<b2Actor2D> RightBorder = make_unique<b2Actor2D>(this, m_WorldId, "RightBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, YBorder, RBorderLocation);
		b2ActorInit(RightBorder, Color(100, 100, 100));

#if 1 // debug floor!
		unique_ptr<b2Actor2D> BotBorder = make_unique<b2Actor2D>(this, m_WorldId, "BotBorder", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, XBorder, DBorderLocation);
		b2ActorInit(BotBorder, Color(100, 100, 100));
#endif 
		unique_ptr<RectangleShape> Background = make_unique<RectangleShape>(Vector2f(ViewportX, ViewportY));
		Background->setTexture(FAssetLoader::FindTexture(&m_AssetLoader, RESOURCES_TEXTURE_BACKGROUND));
		m_RenderShapes.push_back(move(Background));

		unique_ptr<RectangleShape> Scoreboard = make_unique<RectangleShape>(Vector2f(ViewportX, ViewportY * 0.3f));
		Scoreboard->setPosition(0.0f, ViewportY * 0.7f);
		Scoreboard->setTexture(FAssetLoader::FindTexture(&m_AssetLoader, RESOURCES_TEXTURE_CHALKBOARD));
		m_RenderShapes.push_back(move(Scoreboard));

		unique_ptr<RectangleShape> ChargeGaugeMaxUniquePtr = make_unique<RectangleShape>();
		ChargeGaugeMaxUniquePtr->setFillColor(Color(145, 145, 145, 255));
		ChargeGaugeMaxUniquePtr->setSize(Vector2f(160.0f, 8.0f));
		m_ChargeGaugeMax = ChargeGaugeMaxUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		m_RenderShapes.push_back(move(ChargeGaugeMaxUniquePtr));

		unique_ptr<RectangleShape> ChargeGaugeProgressUniquePtr = make_unique<RectangleShape>();
		ChargeGaugeProgressUniquePtr->setFillColor(Color::Yellow);
		ChargeGaugeProgressUniquePtr->setFillColor(Color::Yellow);
		m_ChargeGaugeProgress = ChargeGaugeProgressUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		m_RenderShapes.push_back(move(ChargeGaugeProgressUniquePtr));

		for (int i = 0; i < 2; i++)
		{
			m_AngleIndicators[i].color = (i == 1) ? Color::Cyan : Color::Blue;
		}
		
		// Board
		const float offsetX = ViewportX * 0.98f;
		const float offsetY = ViewportY * 0.35f;
		const Vector2f boardSize(8.0f, 200.0f);
		const Vector2f boardPos(ViewportX * 0.98f, ViewportY * 0.35f);

		unique_ptr<b2Actor2D> BoardFrame1 = make_unique<b2Actor2D>(this, m_WorldId, "board1", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, boardSize, boardPos);
		b2ActorInit(BoardFrame1, Color(40, 40, 40, 255));

		const Vector2f netEdgeSize(8.0f, 90.0f);
		const Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);
		
		unique_ptr<b2Actor2D> BoardFrame2 = make_unique<b2Actor2D>(this, m_WorldId, "board2", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, netEdgeSize, netEdgePos);
		Box2DHelper::SetTransform(BoardFrame2->GetBodyInstance(), Box2DHelper::GetPosition(BoardFrame2->GetBodyInstance()), -0.261799388f);
		b2ActorInit(BoardFrame2, Color(40, 40, 40, 255));
	
		const Vector2f sensorSize(48.0f, 48.0f);
		const Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);
		
		unique_ptr<b2Actor2D> ScoreSensor = make_unique<b2Actor2D>(this, m_WorldId, "sensor", EActorShapeType::Circle, Eb2ShapeType::Circle, sensorSize, sensorPos, 0.0f, false, true);
		ScoreSensor->BindOnBeginoverlap(SensorOverlap);
		b2ActorInit(ScoreSensor, Color(255, 255, 0, 100));

		MakeTrack();
		MakeProjector();
		SetupText();
	}

	return bInitChecks;
}

void Application::Tick(const float DeltaTime)
{
	sf::Event ExitEvent;

	// Delay PollEvent, avoid to use
	while (m_AppWindow.pollEvent(ExitEvent))
	{
		if (ExitEvent.type == sf::Event::Closed)
		{
			m_AppWindow.close();
		}
	}

	m_GameState.Tick();
	m_TextRenderer.Tick();

	// Process contact events
	if (m_b2ActorContactListner)
	{
		m_b2ActorContactListner->ProcessContactEvents(m_WorldId);
	}

	for (auto& i : m_b2Actors)
		if (i) i->Tick();

	for (auto& i : m_Balls)
		if (i) i->Tick();
	
	// Need to update on tick.
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
		// If the game already Started, Do something else.
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
	
	// Middle Button: Reset
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

			for (auto& i : m_Balls)
				i->MakeInactive();
		}
	}
	else
	{
		m_bMiddleMousePressed = false;
	}

	// Update Info Gauge
	float maxVelocity = 60.0f;
	float percentage = m_GameState.GetChargedBallVelocity() / maxVelocity;

	const sf::Vector2f PivotLocation = m_PivotCache->GetLocation();
	const sf::Vector2f MouseLocation = sf::Vector2f(sf::Mouse::getPosition(m_AppWindow));
	const sf::Vector2f OffsetMouseLocation = sf::Vector2f(sf::Mouse::getPosition(m_AppWindow) - sf::Vector2i(16, 16));

	m_ChargeGaugeMax->setPosition(OffsetMouseLocation);
	m_ChargeGaugeMax->setSize(sf::Vector2f(160.0f, 8.0f));
	m_ChargeGaugeProgress->setPosition(OffsetMouseLocation);
	m_ChargeGaugeProgress->setSize(sf::Vector2f(160.0f * percentage, 8.0f));

	// Update Angle Indicator
	m_AngleIndicators[0].position = PivotLocation;
	m_AngleIndicators[1].position = MouseLocation;

	// Rendering
	m_AppWindow.clear(CORNFLOWER_BLUE);

	for (auto& Itr : m_RenderShapes)
		m_AppWindow.draw(*Itr);

	for (auto& Itr : m_b2Actors)
		m_AppWindow.draw(*Itr->GetShape());

	for (auto& Itr : m_Balls)
	{
		m_AppWindow.draw(*Itr->GetShape());
		m_AppWindow.draw(*Itr->m_DebugForward);
	}

	for (auto& Itr : m_TextRenderer.GetTextData())
	{
		if (Itr->m_bIsActive)
			m_AppWindow.draw(Itr->m_Text);
	}

	m_AppWindow.draw(m_AngleIndicators, 2, sf::Lines);
	m_AppWindow.display();
}

void Application::EndPlay()
{
	m_TickHandle.EndTick();
}

void Application::MakeTrack()
{
	using namespace std;
	using namespace sf;

	// The Track 
	const float ViewportX = (float)m_RenderWindowData.m_WindowSize.x;
	const float ViewportY = (float)m_RenderWindowData.m_WindowSize.y;
	const int Row = 14;
	const int Column = 2;

	const Vector2f StartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row * 32.0f));
	const Vector2f Size(32.0f, 32.0f);

	unique_ptr<RectangleShape> Tracks[Row][Column];
	for (int i = 0; i < Row; ++i)
	{
		for (int j = 0; j < Column; ++j)
		{
			Tracks[i][j] = make_unique<RectangleShape>();

			Tracks[i][j]->setSize(Size);
			Tracks[i][j]->setPosition(StartLocation.x + j * 32.0f, StartLocation.y + i * 32.0f);
			Tracks[i][j]->setTexture(FAssetLoader::FindTexture(&m_AssetLoader, RESOURCES_TEXTURE_BOXALT));
			m_RenderShapes.push_back(move(Tracks[i][j]));
		}
	}
}

void Application::MakeProjector()
{
	using namespace std;
	using namespace sf;

	// Collapsed function body. Transfering ownership of local unique ptr to the container
	auto Setup = [this](unique_ptr<b2Actor2D>& p, const Color c) -> void
	{
		p->GetShape()->setOutlineThickness(-1);
		p->GetShape()->setOutlineColor(Color::Black);
		p->GetShape()->setFillColor(c);
		m_b2Actors.push_back(move(p));
	};

	// Projector Pivot
	const int Row = 14;
	const float ViewportX = (float)m_RenderWindowData.m_WindowSize.x;
	const float ViewportY = (float)m_RenderWindowData.m_WindowSize.y;
	const Vector2f StartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row * 32.0f));
	const Vector2f Location(StartLocation.x + 32.0f, StartLocation.y + (Row / 2) * 32.0f);
	unique_ptr<b2Actor2D> Pivot = make_unique<b2Actor2D>(this, m_WorldId, "Pivot", EActorShapeType::Rectangle, Eb2ShapeType::Polygon, Vector2f(8.0f, 8.0f), Location, 0.0f, false, false);
	Pivot->BindOnTick(PivotTick);
	m_PivotCache = Pivot.get();
	Setup(Pivot, Color(0, 0, 255, 100));

	unique_ptr<b2Actor2D> Wheel = make_unique<b2Actor2D>(this, m_WorldId, "Wheel", EActorShapeType::Circle, Eb2ShapeType::Circle, Vector2f(128.0f, 128.0f), Location, 0.0f, false, true);
	Wheel->BindOnTick(WheelTick);
	m_WheelCache = Wheel.get();
	Setup(Wheel, Color(0, 255, 255, 40));
}

void Application::SetupText()
{
	const float Unit = 32.0f;
	const float LineY1 = 530;
	const float LineY2 = 620;
	std::unique_ptr<FTextData> t1 = std::make_unique<FTextData>();
	t1->m_StartLocation = sf::Vector2f(80, LineY1);
	t1->m_Text.setCharacterSize(30);
	t1->m_Font = FAssetLoader::FindFont(&m_AssetLoader, RESOURCES_FONT_CHALK);
	t1->Init();
	m_LevelTextCache = t1.get();
	m_TextRenderer.Add(t1);

	std::unique_ptr<FTextData> t2 = std::make_unique<FTextData>();
	t2->m_StartLocation = sf::Vector2f(80, LineY2);
	t2->m_Text.setCharacterSize(30);
	t2->m_Font = FAssetLoader::FindFont(&m_AssetLoader, RESOURCES_FONT_CHALK);
	t2->Init();
	m_ScoreCache = t2.get();
	m_TextRenderer.Add(t2);

	std::unique_ptr<FTextData> t3 = std::make_unique<FTextData>();
	t3->m_StartLocation = sf::Vector2f(768, LineY1);
	t3->m_Text.setCharacterSize(30);
	t3->m_Font = FAssetLoader::FindFont(&m_AssetLoader, RESOURCES_FONT_CHALK);
	t3->Init();
	m_HiScoreCache = t3.get();
	m_TextRenderer.Add(t3);

	std::unique_ptr<FTextData> t4 = std::make_unique<FTextData>();
	t4->m_StartLocation = sf::Vector2f(768, LineY2);
	t4->m_Text.setCharacterSize(30);
	t4->m_Font = FAssetLoader::FindFont(&m_AssetLoader, RESOURCES_FONT_CHALK);
	t4->Init();
	m_BallCountCache = t4.get();
	m_TextRenderer.Add(t4);

	std::unique_ptr<FTextData> t5 = std::make_unique<FTextData>();
	t5->m_StartLocation = sf::Vector2f(368, LineY1);
	t5->m_Text.setCharacterSize(30);
	t5->m_Font = FAssetLoader::FindFont(&m_AssetLoader, RESOURCES_FONT_CHALK);
	t5->Init();
	m_CountdownTimeCache = t5.get();
	m_TextRenderer.Add(t5);

	std::unique_ptr<FTextData> t6 = std::make_unique<FTextData>();
	t6->m_StartLocation = sf::Vector2f(368, LineY2);
	t6->m_Text.setCharacterSize(30);
	t6->m_Font = FAssetLoader::FindFont(&m_AssetLoader, RESOURCES_FONT_CHALK);
	t6->Init();
	m_ElapsedTimeCache = t6.get();
	m_TextRenderer.Add(t6);

	const float ViewportX = (float)m_RenderWindowData.m_WindowSize.x;
	const float ViewportY = (float)m_RenderWindowData.m_WindowSize.y;
	std::unique_ptr<FTextData> t7 = std::make_unique<FTextData>(); // middle
	t7->m_StartLocation = sf::Vector2f(ViewportX / 2, ViewportY / 2);
	t7->m_EndLocation = sf::Vector2f(ViewportX / 2, -16);
	t7->m_FadeTime = 1.0f;
	t7->m_Text.setCharacterSize(30);
	t7->m_TextData = "SPACE TO START!";
	t7->m_Font = FAssetLoader::FindFont(&m_AssetLoader, RESOURCES_FONT_CHALK);
	t7->m_bIsPaused = true;
	t7->Init();
	m_CenterTextCache = t7.get();
	m_TextRenderer.Add(t7);
}

void Application::SpawnBall()
{
	// Get magnitude of the multiplier.
	const float velocity = m_GameState.GetChargedBallVelocity();

	const sf::Vector2f BallSpawnLocation(m_PivotCache->GetLocation() + sf::Vector2f(32, 32));

	// Construct data to parse.
	Fb2ActorSpawnParam SpawnParam;
	SpawnParam.Package = this;
	SpawnParam.WorldContext = m_WorldId;
	SpawnParam.Name = "Ball";
	SpawnParam.ShapeType = EActorShapeType::Circle;
	SpawnParam.BodyType = Eb2ShapeType::Circle;
	SpawnParam.Size = sf::Vector2f(32, 32);
	SpawnParam.Location = BallSpawnLocation;
	SpawnParam.Rotation = 0.0f;
	SpawnParam.bIsDynamicBody = true;
	SpawnParam.bGenerateOverlaps = false;
	SpawnParam.bAutoActivate = true;

	auto FindPredicate = [](auto& P) -> bool { return !P->IsActive(); };
	auto pActor = std::find_if(m_Balls.begin(), m_Balls.end(), FindPredicate);

	// If found
	if (pActor != m_Balls.end())
	{
		b2Actor2D* const ReuseBall = (*pActor) ? (*pActor).get() : nullptr;
		if (ReuseBall)
		{
			b2BodyId bodyId = ReuseBall->GetBodyInstance();
			Box2DHelper::SetLinearVelocity(bodyId, b2Vec2(velocity, -velocity));
			std::cout << "X: " << Box2DHelper::GetLinearVelocity(bodyId).x << " Y: " << Box2DHelper::GetLinearVelocity(bodyId).y << std::endl;
			ReuseBall->SetInitTransform(SpawnParam.Location, SpawnParam.Rotation);
			ReuseBall->ResetToInitTransform();
			ReuseBall->Activate();
		}
	}
	else
	{
		// Construct new.
		std::unique_ptr<b2Actor2D> Ball = std::make_unique<b2Actor2D>(SpawnParam);
		Ball->GetShape()->setTexture(FAssetLoader::FindTexture(&m_AssetLoader, RESOURCES_TEXTURE_BASKETBALL));
		b2BodyId bodyId = Ball->GetBodyInstance();
		Box2DHelper::SetLinearVelocity(bodyId, b2Vec2(velocity, -velocity));
		std::cout << "X: " << Box2DHelper::GetLinearVelocity(bodyId).x << " Y: " << Box2DHelper::GetLinearVelocity(bodyId).y << std::endl;
		
		// Update shape properties for the ball
		if (b2ShapeDef* shapeDef = Ball->GetShapeDefinition())
		{
			shapeDef->density = 0.83f;
			shapeDef->friction = 0.4f;
			shapeDef->restitution = 0.65f;
		}
		
		Ball->BindOnTick(BallTick);
		// Ball->BindOnBeginoverlap(BallOverlap);
		m_Balls.push_back(std::move(Ball));
	}
}

void Application::PivotTick(b2Actor2D* Actor)
{
	if (!Actor) return;
	// if (!Actor->GetPackage()->GameState.GetIsGameStarted()) return;

	const float ElapsedTime = Actor->GetPackage()->GetTickHandle().GetElapsedTime();
	const float deltaY = 3.0f * cosf(ElapsedTime) / 32.0f;
	b2BodyId bodyId = Actor->GetBodyInstance();
	b2Vec2 Location = Box2DHelper::GetPosition(bodyId) + b2Vec2(0, deltaY);
	Box2DHelper::SetTransform(bodyId, Location, Box2DHelper::GetAngle(bodyId));
}

void Application::WheelTick(b2Actor2D* Actor)
{
	if (!Actor) return;
	// if (!Actor->GetPackage()->GameState.GetIsGameStarted()) return;

	b2Vec2 PivotLocation = Box2DHelper::GetPosition(Actor->GetPackage()->m_PivotCache->GetBodyInstance());
	b2BodyId bodyId = Actor->GetBodyInstance();
	Box2DHelper::SetTransform(bodyId, PivotLocation, Box2DHelper::GetAngle(bodyId));
}

void Application::BallTick(b2Actor2D* Actor)
{
	if (!Actor) return;
	if (!Actor->GetPackage()->GameState.GetIsGameStarted()) return;

	const bool Ax = Actor->GetLocation().x >= Actor->GetPackage()->m_RenderWindowData.m_WindowSize.x + 64.0f;
	const bool Bx = Actor->GetLocation().x <= -64.0f;
	const bool Ay = Actor->GetLocation().y >= Actor->GetPackage()->m_RenderWindowData.m_WindowSize.y + 64.0f;
	const bool By = Actor->GetLocation().y <= -64.0f;

	if (Ax || Bx || Ay || By)
	{
		Actor->MakeInactive();
	}
}

void Application::SensorOverlap(b2Actor2D* OverlapActor)
{
	if (!OverlapActor->GetPackage()->GameState.GetIsGameStarted()) return;

	if (OverlapActor->GetObjectName() == "Ball")
	{
		OverlapActor->GetPackage()->GameState.ScoreBall();
	}
}
