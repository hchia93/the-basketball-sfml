#include "b2Actor2D.h"
#include "Application.h"


b2Actor2D::b2Actor2D(Application* package, b2World* worldContext, const std::string name, EActorShapeType shapeType, const Eb2ShapeType bodyType, sf::Vector2f size, sf::Vector2f location, const float rotation, const bool bIsDynamicBody, const bool bGenerateOverlaps, const bool bAutoActivate)
{
	Construct(package, worldContext, name, shapeType, bodyType, size, location, rotation, bIsDynamicBody, bGenerateOverlaps, bAutoActivate);
}

b2Actor2D::b2Actor2D(const Fb2ActorSpawnParam spawnParam)
{
	Construct(spawnParam.m_Package, spawnParam.m_WorldContext, spawnParam.m_Name, spawnParam.m_ShapeType, spawnParam.m_BodyType,
		spawnParam.m_Size, spawnParam.m_Location, spawnParam.m_Rotation, spawnParam.m_bIsDynamicBody, spawnParam.m_bGenerateOverlaps, spawnParam.m_bAutoActivate);
}

b2Actor2D::~b2Actor2D()
{
}

void b2Actor2D::Construct(Application* package, b2World* worldContext, const std::string name, const EActorShapeType shapeType, const Eb2ShapeType bodyType, sf::Vector2f size, sf::Vector2f location, const float rotation, const bool bIsDynamicBody, const bool bGenerateOverlaps, const bool bAutoActivate)
{
	if (!package)
	{
		return;
	}
	if (!worldContext)
	{
		return;
	}

	this->m_Package = package;

	m_ObjectName = name;
	m_ObjectShapes.m_ShapeType = shapeType;
	m_CollisionType = bodyType;

	MakeShapeInstance(shapeType);
	m_ObjectShapeCache = m_ObjectShapes.Get();
	SetShapeProperties(shapeType, size);
	if (m_ObjectShapes.Get())
	{
		m_ObjectShapes.Get()->setOrigin(size * 0.5f);
		m_ObjectShapes.Get()->setFillColor(sf::Color(128, 128, 128, 255));
		m_ObjectShapes.Get()->setOutlineThickness(1);
		m_ObjectShapes.Get()->setOutlineColor(sf::Color::Black);
	}

	m_BodyDefinition = std::make_unique<b2BodyDef>();
	m_BodyDefinition->position = b2Vec2(location.x / PIXEL_PER_METER, location.y / PIXEL_PER_METER);
	m_BodyDefinition->type = bIsDynamicBody ? b2_dynamicBody : b2_staticBody;

	m_InitialPosition = b2Vec2(location.x / PIXEL_PER_METER, location.y / PIXEL_PER_METER);
	m_InitialRotation = rotation;

	MakeB2ShapeInstance(bodyType);
	SetB2ShapeProperties(bodyType, size);

	if (m_BodyShape)
	{
		m_FixtureDefinition = std::make_unique<b2FixtureDef>();
		m_FixtureDefinition->shape = m_BodyShape.get();
		m_FixtureDefinition->density = 0.5f;
		m_FixtureDefinition->friction = 0.5f;
		m_FixtureDefinition->restitution = 0.5f;
		m_FixtureDefinition->isSensor = bGenerateOverlaps; // change it to MakeSensor and get data.
	}

	m_BodyInstance = worldContext->CreateBody(m_BodyDefinition.get());
	m_BodyInstance->CreateFixture(m_FixtureDefinition.get());
	m_BodyInstance->SetUserData(this);

	this->m_bGenerateOverlaps = bGenerateOverlaps;
	m_bIsDynamicObject = bIsDynamicBody;

	if (bAutoActivate)
	{
		Activate();
	}

	// Debug!
	m_DebugForward = std::make_unique<sf::RectangleShape>();
	m_DebugForward->setOrigin(size * 0.5f);
	m_DebugForward->setSize(sf::Vector2f(size.x + 20.0f, 5));
	m_DebugForward->setFillColor(sf::Color(255, 0, 0, 255));
}

void b2Actor2D::Tick()
{
	if (m_bIsActive)
	{
		// Box2D uses radians for rotation, SFML uses degree
		// Snap rendering according to Box2D BodyInstance.
		m_ObjectShapes.Get()->setRotation(m_BodyInstance->GetAngle() * 180 / b2_pi);
		m_ObjectShapes.Get()->setPosition(m_BodyInstance->GetPosition().x * PIXEL_PER_METER, m_BodyInstance->GetPosition().y * PIXEL_PER_METER);
		m_DebugForward->setRotation(m_BodyInstance->GetAngle() * 180 / b2_pi);
		m_DebugForward->setPosition(m_BodyInstance->GetPosition().x * PIXEL_PER_METER, m_BodyInstance->GetPosition().y * PIXEL_PER_METER);

		if (m_TickCallback != nullptr)
		{
			m_TickCallback(this);
		}
	}
}


void b2Actor2D::ResetToInitTransform()
{
	if (m_BodyInstance)
	{
		m_BodyInstance->SetTransform(m_InitialPosition, m_InitialRotation);
	}
	else
	{
		LOG("No BodyInstance isn't created.");
		LOG(" ");
	}
}

void b2Actor2D::BeginOverlap(b2Actor2D* overlappedActor)
{
	if (m_bGenerateOverlaps && overlappedActor)
	{
		if (m_OnBeginOverlapCallback)
		{
			m_OnBeginOverlapCallback(overlappedActor);
		}
	}
}

void b2Actor2D::EndOverlap(b2Actor2D* overlappedActor)
{
	if (m_bGenerateOverlaps && overlappedActor)
	{
		if (m_OnEndOverlapCallback)
		{
			m_OnEndOverlapCallback(overlappedActor);
		}
	}
}

void b2Actor2D::BindOnBeginoverlap(void (*callback)(b2Actor2D* overlappedActor))
{
	m_OnBeginOverlapCallback = callback;
}

void b2Actor2D::BindOnEndOverlap(void (*callback)(b2Actor2D* overlappedActor))
{
	m_OnEndOverlapCallback = callback;
}

void b2Actor2D::BindOnTick(void (*tickFunction)(b2Actor2D* actor))
{
	m_TickCallback = tickFunction;
}

void b2Actor2D::MakeShapeInstance(const EActorShapeType shapeType)
{
	switch (shapeType)
	{
		case EActorShapeType::Rectangle:
			m_ObjectShapes.m_RectangleShape = std::make_unique<sf::RectangleShape>();
			break;
		case EActorShapeType::Circle:
			m_ObjectShapes.m_CircleShape = std::make_unique<sf::CircleShape>();
			break;
		case EActorShapeType::Convex:
			m_ObjectShapes.m_ConvexShape = std::make_unique<sf::ConvexShape>();
			break;
	}

	// Prevent spawn at 0,0,0 at being visible before the first tick update.
	m_ObjectShapes.Get()->setPosition(sf::Vector2f(-200, -200));
}

void b2Actor2D::SetShapeProperties(const EActorShapeType shapeType, sf::Vector2f size)
{
	if (!m_ObjectShapeCache)
	{
		m_ObjectShapeCache = m_ObjectShapes.Get();
	}
	switch (shapeType)
	{
		case EActorShapeType::Circle:
			if (sf::CircleShape* const p = dynamic_cast<sf::CircleShape*>(m_ObjectShapeCache))
			{
				p->setRadius(size.x / 2);
			}
			break;
		case EActorShapeType::Rectangle:
			if (sf::RectangleShape* const p = dynamic_cast<sf::RectangleShape*>(m_ObjectShapeCache))
			{
				p->setSize(size);
			}
			break;
		case EActorShapeType::Convex:
			// Not implemented
			break;
	}
}

void b2Actor2D::MakeB2ShapeInstance(const Eb2ShapeType bodyType)
{
	switch (bodyType)
	{
		case Eb2ShapeType::Chain:
			m_BodyShape = std::make_unique<b2ChainShape>();
			break;
		case Eb2ShapeType::Edge:
			m_BodyShape = std::make_unique<b2EdgeShape>();
			break;
		case Eb2ShapeType::Polygon:
			m_BodyShape = std::make_unique<b2PolygonShape>();
			break;
		case Eb2ShapeType::Circle:
			m_BodyShape = std::make_unique<b2CircleShape>();
			break;
	}
}

void b2Actor2D::SetB2ShapeProperties(const Eb2ShapeType bodyType, sf::Vector2f size)
{
	if (!m_BodyShape)
	{
		return;
	}
	switch (bodyType)
	{
		case Eb2ShapeType::Chain:
		case Eb2ShapeType::Edge:
			//Not supported yet.
			break;
		case Eb2ShapeType::Polygon:
		{
			if (b2PolygonShape* const p = dynamic_cast<b2PolygonShape*>(m_BodyShape.get()))
			{
				p->SetAsBox((size.x * 0.5f) / PIXEL_PER_METER, (size.y * 0.5f) / PIXEL_PER_METER);
			}
			break;
		}
		case Eb2ShapeType::Circle:
		{
			if (b2CircleShape* const p = dynamic_cast<b2CircleShape*>(m_BodyShape.get()))
			{
				p->m_radius = (size.x * 0.5f) / PIXEL_PER_METER;
			}
			break;
		}
	}
}

void b2Actor2D::Activate()
{
	m_bIsActive = true;
	m_BodyInstance->SetActive(true);
	m_BodyInstance->SetAwake(true);
}

void b2Actor2D::MakeInactive()
{
	m_bIsActive = false;
	
	// Move it out of screen.
	BodyInstance->SetActive(false);
	BodyInstance->SetAwake(false);

	ObjectShapes.Get()->setPosition(SFML::Vector2f(-200, -200));
	DebugForward->setPosition(SFML::Vector2f(-200, -200));
}