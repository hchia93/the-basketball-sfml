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
	while (AppWindow.isOpen())
	{
		TickHandle.BeginTick();
	}

	EndPlay();
}

int Application::Initialize()
{
	// Reduce the code length, scope in this function only.
	using namespace sf;
	using namespace std;

	bool bInitChecks = true;
	bInitChecks &= TickHandle.BindApplication(this);
	bInitChecks &= GameState.BindApplication(this);
	bInitChecks &= TextRenderer.BindApplication(this);
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
		AppWindow.create(RenderWindowData.GetVideoModeFromData(), RenderWindowData.GetWindowName());
		AppWindow.setVerticalSyncEnabled(true);
		//AppWindow.setActive();

		// Border creations
		const float BorderThickness = 16.0f;
		const float ViewportX = (float)RenderWindowData.Width;
		const float ViewportY = (float)RenderWindowData.Height;
		const Vector2f XBorder(ViewportX, BorderThickness);
		const Vector2f YBorder(BorderThickness, ViewportY * 0.7f);
		const Vector2f UBorderLocation(ViewportX * 0.5f						, BorderThickness * 0.5f);
		const Vector2f DBorderLocation(ViewportX * 0.5f						, ViewportY - BorderThickness * 0.5f);
		const Vector2f LBorderLocation(BorderThickness * 0.5f				, ViewportY * 0.5f - (ViewportY * .15f) ); // 1 - .7f div 2
		const Vector2f RBorderLocation(ViewportX - BorderThickness * 0.5f	, ViewportY * 0.5f - (ViewportY * .15f) ); // 1 - .7f div 2

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
		unique_ptr<RectangleShape> Background = make_unique<RectangleShape>(Vector2f(ViewportX, ViewportY));
		Background->setTexture(m_AssetLoader.FindTexture(RESOURCES_TEXTURE_BACKGROUND));
		RenderShapes.push_back(move(Background));

		unique_ptr<RectangleShape> Scoreboard = make_unique<RectangleShape>(Vector2f(ViewportX, ViewportY * .3f));
		Scoreboard->setPosition({0.0f, ViewportY * 0.7f});
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
		const float offsetX = ViewportX * 0.98f;
		const float offsetY = ViewportY * 0.35f;
		const Vector2f boardSize(8.0f, 200.0f);
		const Vector2f boardPos(ViewportX * 0.98f, ViewportY * 0.35f);

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

void Application::Tick(const float DeltaTime)
{
	std::optional<sf::Event> event = AppWindow.pollEvent();
	if (event.has_value() && event.value().is<sf::Event::Closed>())
	{
		AppWindow.close();
	}

	GameState.Tick();
	TextRenderer.Tick();

	for (auto&& element : b2Actors)
	{
		if (element)
		{
			element->Tick();
		}
	}
		

	for (auto& element : Balls)
	{
		if (element)
		{
			element->Tick();
		}
	}
	
	// Need to update on tick.
	LevelTextCache->Text->setString("LEVEL\n" + GameState.GetLevelString());
	ScoreCache->Text->setString("SCORE\n" + GameState.GetScoreString());
	HiScoreCache->Text->setString("HISCORE\n" + GameState.GetHiScoreString());
	BallCountCache->Text->setString("REQ. BALL\n" + GameState.GetReqBallString());
	CountdownTimeCache->Text->setString("REMAINING TIME\n" + GameState.GetRemainingTimeString() + " S");
	ElapsedTimeCache->Text->setString("ELAPSED MIN\n" + GameState.GetElapsedTimeMinString() + " M" + GameState.GetElapsedTimeSecondString() + " S");

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		if (!GameState.GetIsGameStarted())
		{
			GameState.StartGame();
			CenterTextCache->bIsPaused = false;
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
			TickHandle.ClearTimer();
			CenterTextCache->Init();
			CenterTextCache->bIsActive = true;
			CenterTextCache->bIsPaused = true;
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
	

	// Update Info Gauge
	float maxVelocity = 60.0f;
	float percentage = GameState.GetChargedBallVelocity() / maxVelocity;

	const sf::Vector2f PivotLocation = PivotCache->GetLocation();
	const sf::Vector2f MouseLocation = sf::Vector2f(sf::Mouse::getPosition(AppWindow));
	const sf::Vector2f OffsetMouseLocation = sf::Vector2f(sf::Vector2i(sf::Mouse::getPosition(AppWindow)) - sf::Vector2i(16, 16));

	ChargeGaugeMax->setPosition(OffsetMouseLocation);
	ChargeGaugeMax->setSize({160.0f, 8.0f});
	ChargeGaugeProgress->setPosition(OffsetMouseLocation);
	ChargeGaugeProgress->setSize(sf::Vector2f(160.0f * percentage, 8.0f));;

	// Update Angle Indicator
	AngleIndicators[0].position = PivotLocation;
	AngleIndicators[1].position = MouseLocation;

	// Rendering
	AppWindow.clear(CORNFLOWER_BLUE);

	for (auto& Itr : RenderShapes)
		AppWindow.draw(*Itr);


	for (auto& Itr : b2Actors)
		AppWindow.draw(*Itr->GetShape());

	for (auto& Itr : Balls)
	{
		AppWindow.draw(*Itr->GetShape());
		AppWindow.draw(*Itr->DebugForward);
	}

	for (auto& Itr : TextRenderer.GetTextData())
	{
		if(Itr->bIsActive)
			AppWindow.draw(*Itr->Text);
	}

	AppWindow.draw(AngleIndicators, 2, sf::PrimitiveType::Lines);
	AppWindow.display();
}

void Application::EndPlay()
{
	TickHandle.EndTick();
}

void Application::MakeTrack()
{
	using namespace std;
	using namespace sf;

	// The Track 
	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;
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
	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;
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
	sf::Font* pChalkFont = m_AssetLoader.FindFont(RESOURCES_FONT_CHALK);
	sf::Font* pPixelFont = m_AssetLoader.FindFont(RESOURCES_FONT_PIXEL);

	const float Unit = 32.0f;
	const float LineY1 = 530;
	const float LineY2 = 620;
	std::unique_ptr<FTextData> t1 = std::make_unique<FTextData>();
	t1->StartLocation = sf::Vector2f(80, LineY1);
	t1->CharacterSize = 30;
	t1->Font = pChalkFont;
	t1->Init();
	LevelTextCache = t1.get();
	TextRenderer.Add(t1);

	std::unique_ptr<FTextData> t2 = std::make_unique<FTextData>();
	t2->StartLocation = sf::Vector2f(80, LineY2);
	t2->CharacterSize = 30;
	t2->Font = pChalkFont;
	t2->Init();
	ScoreCache = t2.get();
	TextRenderer.Add(t2);

	std::unique_ptr<FTextData> t3 = std::make_unique<FTextData>();
	t3->StartLocation = sf::Vector2f(768, LineY1);
	t3->CharacterSize = 30;
	t3->Font = pChalkFont;
	t3->Init();
	HiScoreCache = t3.get();
	TextRenderer.Add(t3);

	std::unique_ptr<FTextData> t4 = std::make_unique<FTextData>();
	t4->StartLocation = sf::Vector2f(768, LineY2);
	t4->CharacterSize = 30;
	t4->Font = pChalkFont;
	t4->Init();
	BallCountCache = t4.get();
	TextRenderer.Add(t4);

	std::unique_ptr<FTextData> t5 = std::make_unique<FTextData>();
	t5->StartLocation = sf::Vector2f(368, LineY1);
	t5->CharacterSize = 30;
	t5->Font = pChalkFont;
	t5->Init();
	CountdownTimeCache = t5.get();
	TextRenderer.Add(t5);

	std::unique_ptr<FTextData> t6 = std::make_unique<FTextData>();
	t6->StartLocation = sf::Vector2f(368, LineY2);
	t6->CharacterSize = 30;
	t6->Font = pChalkFont;
	t6->Init();
	ElapsedTimeCache = t6.get();
	TextRenderer.Add(t6);

	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;
	std::unique_ptr<FTextData> t7 = std::make_unique<FTextData>(); // middle
	t7->StartLocation = sf::Vector2f(ViewportX/2, ViewportY/2);
	t7->EndLocation = sf::Vector2f(ViewportX / 2, -16);
	t7->FadeTime = 1.0f;
	t7->CharacterSize = 30;
	t7->TextData = "PRESS SPACE BAR TO START";
	t7->Font = pPixelFont;
	t7->bIsPaused = true;
	t7->Init();
	CenterTextCache = t7.get();
	TextRenderer.Add(t7);
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
	if (!Actor) return;
	//if (!Actor->GetPackage()->GameState.GetIsGameStarted()) return;

	const float ElapsedTime = Actor->GetPackage()->GetTickHandle().GetElapsedTime();
	const float deltaY = 3.0f * cosf(ElapsedTime) / 32.0f;
	b2Vec2 Location = Actor->GetBodyInstance()->GetPosition() + b2Vec2(0, deltaY);
	Actor->GetBodyInstance()->SetTransform(Location, Actor->GetBodyInstance()->GetAngle());
}

void Application::WheelTick(b2Actor2D* Actor)
{
	if (!Actor) return;
	//if (!Actor->GetPackage()->GameState.GetIsGameStarted()) return;

	b2Vec2 PivotLocation = Actor->GetPackage()->PivotCache->GetBodyInstance()->GetPosition();
	Actor->GetBodyInstance()->SetTransform(PivotLocation, Actor->GetBodyInstance()->GetAngle());
}

void Application::BallTick(b2Actor2D* Actor)
{
	if (!Actor) return;
	if (!Actor->GetPackage()->GameState.GetIsGameStarted()) return;

	const bool Ax = Actor->GetLocation().x >= Actor->GetPackage()->RenderWindowData.Width + 64.0f;
	const bool Bx = Actor->GetLocation().x <= -64.0f;
	const bool Ay = Actor->GetLocation().y >= Actor->GetPackage()->RenderWindowData.Height + 64.0f;
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
