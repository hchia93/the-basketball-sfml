#include "b2Actor2D.h"
#include "Application.h"


b2Actor2D::b2Actor2D(Application* Package, b2World* WorldContext, const std::string Name, EActorShapeType ShapeType, const Eb2ShapeType BodyType, SFML::Vector2f Size, SFML::Vector2f Location, const float Rotation, const bool bIsDynamicBody, const bool bGenerateOverlaps, const bool bAutoActivate)
{
	Construct(Package, WorldContext, Name, ShapeType, BodyType, Size, Location, Rotation, bIsDynamicBody, bGenerateOverlaps, bAutoActivate);
}

b2Actor2D::b2Actor2D(const Fb2ActorSpawnParam SpawnParam)
{	
	Construct(SpawnParam.Package, SpawnParam.WorldContext, SpawnParam.Name, SpawnParam.ShapeType, SpawnParam.BodyType, 
		SpawnParam.Size, SpawnParam.Location, SpawnParam.Rotation, SpawnParam.bIsDynamicBody, SpawnParam.bGenerateOverlaps, SpawnParam.bAutoActivate);
}

b2Actor2D::~b2Actor2D() 
{
}

void b2Actor2D::Construct(Application * Package, b2World * WorldContext, const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, SFML::Vector2f Size, SFML::Vector2f Location, const float Rotation, const bool bIsDynamicBody, const bool bGenerateOverlaps, const bool bAutoActivate)
{
	if (!Package) return;
	if (!WorldContext) return;

	this->Package = Package;

	ObjectName = Name;
	ObjectShapes.ShapeType = ShapeType;
	CollisionType = BodyType;

	MakeShapeInstance(ShapeType);
	ObjectShapeCache = ObjectShapes.Get();
	SetShapeProperties(ShapeType, Size);
	if (ObjectShapes.Get())
	{
		ObjectShapes.Get()->setOrigin(Size * 0.5f);
		ObjectShapes.Get()->setFillColor(SFML::Color(128, 128, 128, 255));
		ObjectShapes.Get()->setOutlineThickness(1);
		ObjectShapes.Get()->setOutlineColor(sf::Color::Black);
	}

	BodyDefinition = std::make_unique<b2BodyDef>();
	BodyDefinition->position = b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER);
	BodyDefinition->type = bIsDynamicBody ? b2_dynamicBody : b2_staticBody;

	InitialPosition = b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER);
	InitialRotation = Rotation;

	MakeB2ShapeInstance(BodyType);
	SetB2ShapeProperties(BodyType, Size);

	if (BodyShape)
	{
		FixtureDefinition = std::make_unique<b2FixtureDef>();
		FixtureDefinition->shape = BodyShape.get();
		FixtureDefinition->density = 0.5f;
		FixtureDefinition->friction = 0.5f;
		FixtureDefinition->restitution = 0.5f;
		FixtureDefinition->isSensor = bGenerateOverlaps; // change it to MakeSensor and get data.
	}

	BodyInstance = WorldContext->CreateBody(BodyDefinition.get());
	BodyInstance->CreateFixture(FixtureDefinition.get());
	BodyInstance->SetUserData(this);

	this->bGenerateOverlaps = bGenerateOverlaps;
	bIsDynamicObject = bIsDynamicBody;

	if (bAutoActivate)
	{
		Activate();
	}

	// Debug!
	DebugForward = std::make_unique<SFML::RectangleShape>();
	DebugForward->setOrigin(Size * 0.5f);
	DebugForward->setSize(SFML::Vector2f(Size.x + 20.0f, 5));
	DebugForward->setFillColor(SFML::Color(255, 0, 0, 255));

}

void b2Actor2D::Tick()
{
	if (bIsActive)
	{
		// Box2D uses radians for rotation, SFML uses degree
		// Snap rendering according to Box2D BodyInstance.
		ObjectShapes.Get()->setRotation(BodyInstance->GetAngle() * 180 / b2_pi);
		ObjectShapes.Get()->setPosition(BodyInstance->GetPosition().x*PIXEL_PER_METER, BodyInstance->GetPosition().y*PIXEL_PER_METER);
		DebugForward->setRotation(BodyInstance->GetAngle() * 180 / b2_pi);
		DebugForward->setPosition(BodyInstance->GetPosition().x*PIXEL_PER_METER, BodyInstance->GetPosition().y*PIXEL_PER_METER);

		if (TickCallback != 0)
		{
			TickCallback(this);
		}
	}
}


void b2Actor2D::ResetToInitTransform()
{
	if (BodyInstance)
	{
		BodyInstance->SetTransform(InitialPosition, InitialRotation);
	}
	else
	{
		LOG("No BodyInstance isn't created.");
		LOG(" ");
	}
}

void b2Actor2D::BeginOverlap(b2Actor2D* OverlappedActor)
{
	if (bGenerateOverlaps && OverlappedActor)
	{
		if (OnBeginOverlapCallback)
		{
			OnBeginOverlapCallback(OverlappedActor);
		}
	}
}

void b2Actor2D::EndOverlap(b2Actor2D* OverlappedActor)
{
	if (bGenerateOverlaps && OverlappedActor)
	{
		if (OnEndOverlapCallback)
		{
			OnEndOverlapCallback(OverlappedActor);
		}
	}
}

void b2Actor2D::BindOnBeginoverlap(void (*Callback)(b2Actor2D* OverlappedActor))
{
	OnBeginOverlapCallback = Callback;
}

void b2Actor2D::BindOnEndOverlap(void (*Callback)(b2Actor2D* OverlappedActor))
{
	OnEndOverlapCallback = Callback;
}

void b2Actor2D::BindOnTick(void(*TickFunction)(b2Actor2D* Actor))
{
	TickCallback = TickFunction;
}

void b2Actor2D::MakeShapeInstance(const EActorShapeType ShapeType)
{
	switch (ShapeType)
	{
		case EActorShapeType::EST_Rectangle:	ObjectShapes.RectangleShape =	std::make_unique<SFML::RectangleShape>(); 		break;
		case EActorShapeType::EST_Circle:		ObjectShapes.CircleShape	=	std::make_unique<SFML::CircleShape>();			break;
		case EActorShapeType::EST_Convex:		ObjectShapes.ConvexShape	=	std::make_unique<SFML::ConvexShape>();			break;
	}

	// Prevent spawn at 0,0,0 at being visible before the first tick update.
	ObjectShapes.Get()->setPosition(SFML::Vector2f(-200, -200));
}

void b2Actor2D::SetShapeProperties(const EActorShapeType ShapeType, SFML::Vector2f Size)
{
	using namespace SFML;

	if (!ObjectShapeCache) ObjectShapeCache = ObjectShapes.Get();
	switch (ShapeType)
	{
		case EActorShapeType::EST_Circle:
			if (CircleShape* const p = dynamic_cast<CircleShape*>(ObjectShapeCache))
			{
				p->setRadius(Size.x/2);
			}
			break;
		case EActorShapeType::EST_Rectangle:
			if (RectangleShape* const p = dynamic_cast<RectangleShape*>(ObjectShapeCache))
			{
				p->setSize(Size);
			}
			break;
	}
}

void b2Actor2D::MakeB2ShapeInstance(const Eb2ShapeType BodyType)
{
	switch (BodyType)
	{
		case Eb2ShapeType::ECT_Chain:		BodyShape = std::make_unique<b2ChainShape>();	break;
		case Eb2ShapeType::ECT_Edge:		BodyShape = std::make_unique<b2EdgeShape>();	break;
		case Eb2ShapeType::ECT_Polygon:		BodyShape = std::make_unique<b2PolygonShape>(); break;
		case Eb2ShapeType::ECT_Circle:		BodyShape = std::make_unique<b2CircleShape>();	break;
	}
}

void b2Actor2D::SetB2ShapeProperties(const Eb2ShapeType BodyType, SFML::Vector2f Size)
{
	if (!BodyShape) return;
	switch (BodyType)
	{
		case Eb2ShapeType::ECT_Chain:		
		case Eb2ShapeType::ECT_Edge:		
			//Not supported yet.
			break;
		case Eb2ShapeType::ECT_Polygon:	
		{
			if(b2PolygonShape* const p = dynamic_cast<b2PolygonShape*>(BodyShape.get()))
			{
				p->SetAsBox((Size.x * 0.5f) / PIXEL_PER_METER, (Size.y * 0.5f) / PIXEL_PER_METER);
			}
			
			break;
		}
			
		case Eb2ShapeType::ECT_Circle:		
		{
			if (b2CircleShape* const p = dynamic_cast<b2CircleShape*>(BodyShape.get()))
			{
				p->m_radius = (Size.x * 0.5f) / PIXEL_PER_METER;
			}

			break;
		}
	}
}

void b2Actor2D::Activate()
{
	bIsActive = true; 
	BodyInstance->SetActive(true);
	BodyInstance->SetAwake(true);
}

void b2Actor2D::MakeInactive()
{
	bIsActive = false;
	
	// Move it out of screen.
	BodyInstance->SetActive(false);
	BodyInstance->SetAwake(false);

	ObjectShapes.Get()->setPosition(SFML::Vector2f(-200, -200));
	DebugForward->setPosition(SFML::Vector2f(-200, -200));
}