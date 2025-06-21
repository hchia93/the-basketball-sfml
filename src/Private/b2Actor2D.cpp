#include "b2Actor2D.h"
#include "Application.h"


b2Actor2D::b2Actor2D(Application* Package, b2WorldId WorldContext, const std::string Name, EActorShapeType ShapeType, const Eb2ShapeType BodyType, sf::Vector2f Size, sf::Vector2f Location, const float Rotation, const bool bIsDynamicBody, const bool bGenerateOverlaps, const bool bAutoActivate)
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

void b2Actor2D::Construct(Application * Package, b2WorldId WorldContext, const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, sf::Vector2f Size, sf::Vector2f Location, const float Rotation, const bool bIsDynamicBody, const bool bGenerateOverlaps, const bool bAutoActivate)
{
	if (!Package) return;
	if (!Box2DHelper::IsValid(WorldContext)) return;

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
		ObjectShapes.Get()->setFillColor(sf::Color(128, 128, 128, 255));
		ObjectShapes.Get()->setOutlineThickness(1);
		ObjectShapes.Get()->setOutlineColor(sf::Color::Black);
	}

	BodyDefinition = std::make_unique<b2BodyDef>();
	BodyDefinition->position = b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER);
	BodyDefinition->type = bIsDynamicBody ? b2_dynamicBody : b2_staticBody;

	InitialPosition = b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER);
	InitialRotation = Rotation;

	// Create the body first
	BodyInstance = Box2DHelper::CreateBody(WorldContext, *BodyDefinition);
	
	// Set up shape definition
	ShapeDefinition = std::make_unique<b2ShapeDef>();
	ShapeDefinition->density = 0.5f;
	ShapeDefinition->friction = 0.5f;
	ShapeDefinition->restitution = 0.5f;
	ShapeDefinition->isSensor = bGenerateOverlaps;

	// Create the shape based on type
	MakeB2ShapeInstance(BodyType);
	SetB2ShapeProperties(BodyType, Size);

	// Set user data for contact handling
	Box2DHelper::SetUserData(BodyInstance, this);

	this->bGenerateOverlaps = bGenerateOverlaps;
	bIsDynamicObject = bIsDynamicBody;

	if (bAutoActivate)
	{
		Activate();
	}

	// Debug!
	DebugForward = std::make_unique<sf::RectangleShape>();
	DebugForward->setOrigin(Size * 0.5f);
	DebugForward->setSize(sf::Vector2f(Size.x + 20.0f, 5));
	DebugForward->setFillColor(sf::Color(255, 0, 0, 255));

}

void b2Actor2D::Tick()
{
	if (bIsActive && Box2DHelper::IsValid(BodyInstance))
	{
		// Box2D uses radians for rotation, SFML uses degree
		// Snap rendering according to Box2D BodyInstance.
		ObjectShapes.Get()->setRotation(Box2DHelper::GetAngle(BodyInstance) * 180 / b2_pi);
		ObjectShapes.Get()->setPosition(Box2DHelper::GetPosition(BodyInstance).x*PIXEL_PER_METER, Box2DHelper::GetPosition(BodyInstance).y*PIXEL_PER_METER);
		DebugForward->setRotation(Box2DHelper::GetAngle(BodyInstance) * 180 / b2_pi);
		DebugForward->setPosition(Box2DHelper::GetPosition(BodyInstance).x*PIXEL_PER_METER, Box2DHelper::GetPosition(BodyInstance).y*PIXEL_PER_METER);

		if (TickCallback != 0)
		{
			TickCallback(this);
		}
	}
}


void b2Actor2D::ResetToInitTransform()
{
	if (Box2DHelper::IsValid(BodyInstance))
	{
		Box2DHelper::SetTransform(BodyInstance, InitialPosition, InitialRotation);
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
		case EActorShapeType::EST_Rectangle:	ObjectShapes.RectangleShape =	std::make_unique<sf::RectangleShape>(); 		break;
		case EActorShapeType::Circle:		ObjectShapes.CircleShape	=	std::make_unique<sf::CircleShape>();			break;
		case EActorShapeType::EST_Convex:		ObjectShapes.ConvexShape	=	std::make_unique<sf::ConvexShape>();			break;
	}

	// Prevent spawn at 0,0,0 at being visible before the first tick update.
	ObjectShapes.Get()->setPosition(sf::Vector2f(-200, -200));
}

void b2Actor2D::SetShapeProperties(const EActorShapeType ShapeType, sf::Vector2f Size)
{
	using namespace sf;

	if (!ObjectShapeCache) ObjectShapeCache = ObjectShapes.Get();
	switch (ShapeType)
	{
		case EActorShapeType::Circle:
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
	// In Box2D 3.x, shapes are created directly on the body
	// This function is now handled in SetB2ShapeProperties
}

void b2Actor2D::SetB2ShapeProperties(const Eb2ShapeType BodyType, sf::Vector2f Size)
{
	if (!ShapeDefinition) return;
	
	switch (BodyType)
	{
		case Eb2ShapeType::ECT_Chain:		
		case Eb2ShapeType::ECT_Edge:		
			//Not supported yet.
			break;
		case Eb2ShapeType::Polygon:	
		{
			// Create box shape
			Box2DHelper::CreateBoxShape(BodyInstance, *ShapeDefinition, 
				(Size.x * 0.5f) / PIXEL_PER_METER, (Size.y * 0.5f) / PIXEL_PER_METER);
			break;
		}
			
		case Eb2ShapeType::ECT_Circle:		
		{
			// Create circle shape
			Box2DHelper::CreateCircleShape(BodyInstance, *ShapeDefinition, 
				(Size.x * 0.5f) / PIXEL_PER_METER);
			break;
		}
	}
}

void b2Actor2D::Activate()
{
	bIsActive = true; 
	Box2DHelper::SetActive(BodyInstance, true);
	Box2DHelper::SetAwake(BodyInstance, true);
}

void b2Actor2D::MakeInactive()
{
	bIsActive = false;
	
	// Move it out of screen.
	Box2DHelper::SetActive(BodyInstance, false);
	Box2DHelper::SetAwake(BodyInstance, false);

	ObjectShapes.Get()->setPosition(sf::Vector2f(-200, -200));
	DebugForward->setPosition(sf::Vector2f(-200, -200));
}