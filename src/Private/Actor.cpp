#include "Actor.h"
#include "Application.h"
#include "ActorPhysicComponent.h"
#include "ActorContactListener.h"

Actor::Actor(Application* package, b2World* worldContext, const std::string& name, EActorShapeType shapeType, Eb2ShapeType bodyType, const sf::Vector2f& size, const sf::Vector2f& location, float rotation, bool isDynamicBody, bool generateOverlaps, bool autoActivate)
	: m_Package(package)
	, m_ObjectName(name)
	, m_IsActive(autoActivate)
	, m_IsDynamicObject(isDynamicBody)
{
	m_ActorRenderShape.CreateShape(shapeType);
	// Set size and origin for visibility
	sf::Shape* shape = m_ActorRenderShape.Get();
	if (shape) {
		switch (shapeType) {
			case EActorShapeType::Rectangle:
				static_cast<sf::RectangleShape*>(shape)->setSize(size);
				shape->setOrigin(sf::Vector2f(size.x * 0.5f, size.y * 0.5f));
				break;
			case EActorShapeType::Circle:
				static_cast<sf::CircleShape*>(shape)->setRadius(size.x * 0.5f);
				shape->setOrigin(sf::Vector2f(size.x * 0.5f, size.y * 0.5f));
				break;
			default:
				break;
		}
	}
	// Initialize debug shape
	m_DebugForward = std::make_unique<sf::RectangleShape>(sf::Vector2f(20.f, 5.f));
	m_DebugForward->setFillColor(sf::Color::Red);
	m_DebugForward->setOrigin(sf::Vector2f(0.f, 2.5f)); // Left edge centered vertically

	m_ActorPhysicBody.Construct(worldContext, bodyType, size, location, rotation, isDynamicBody, generateOverlaps);
	
	if (m_ActorPhysicBody.GetBodyInstance())
	{
		m_ActorPhysicBody.GetBodyInstance()->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
	}
}

Actor::Actor(const FActorSpawnParam& spawnParam)
	: m_Package(spawnParam.package)
	, m_ObjectName(spawnParam.name)
	, m_IsActive(spawnParam.autoActivate)
	, m_IsDynamicObject(spawnParam.isDynamicBody)
{
	m_ActorRenderShape.CreateShape(spawnParam.shapeType);
	// Set size and origin for visibility
	sf::Shape* shape = m_ActorRenderShape.Get();
	if (shape) {
		switch (spawnParam.shapeType) {
			case EActorShapeType::Rectangle:
				static_cast<sf::RectangleShape*>(shape)->setSize(spawnParam.size);
				shape->setOrigin(sf::Vector2f(spawnParam.size.x * 0.5f, spawnParam.size.y * 0.5f));
				break;
			case EActorShapeType::Circle:
				static_cast<sf::CircleShape*>(shape)->setRadius(spawnParam.size.x * 0.5f);
				shape->setOrigin(sf::Vector2f(spawnParam.size.x * 0.5f, spawnParam.size.y * 0.5f));
				break;
			default:
				break;
		}
	}
	// Initialize debug shape
	m_DebugForward = std::make_unique<sf::RectangleShape>(sf::Vector2f(20.f, 5.f));
	m_DebugForward->setFillColor(sf::Color::Red);
	m_DebugForward->setOrigin(sf::Vector2f(0.f, 2.5f)); // Left edge centered vertically

	m_ActorPhysicBody.Construct(spawnParam.worldContext, spawnParam.bodyType, spawnParam.size, spawnParam.location, spawnParam.rotation, spawnParam.isDynamicBody, spawnParam.generateOverlaps);
	
	if (m_ActorPhysicBody.GetBodyInstance())
	{
		m_ActorPhysicBody.GetBodyInstance()->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
	}
}

Actor::~Actor()
{
	if (m_ActorPhysicBody.GetBodyInstance())
	{
		m_ActorPhysicBody.Destroy(m_ActorPhysicBody.GetBodyInstance()->GetWorld());
	}
}

void Actor::Tick()
{
	if (!m_IsActive) return;
	
	// Update render shape position from physics body
	if (m_ActorPhysicBody.GetBodyInstance() && m_ActorRenderShape.Get())
	{
		b2Vec2 position = m_ActorPhysicBody.GetBodyInstance()->GetPosition();
		float angle = m_ActorPhysicBody.GetBodyInstance()->GetAngle();
		
		sf::Vector2f sfPosition(position.x * 32.0f, position.y * 32.0f);
		sf::Angle sfAngle = sf::degrees(angle * 180.0f / 3.14159f);
		
		m_ActorRenderShape.Get()->setPosition(sfPosition);
		m_ActorRenderShape.Get()->setRotation(sfAngle);
		
		// Update debug forward shape
		if (m_DebugForward)
		{
			m_DebugForward->setPosition(sfPosition);
			m_DebugForward->setRotation(sfAngle);
		}
	}
	
	// Call custom tick function if bound
	if (m_OnTickCallback)
	{
		m_OnTickCallback(this);
	}
}

void Actor::BindOnTick(std::function<void(Actor*)> callback)
{
	m_OnTickCallback = callback;
}

void Actor::BindOnBeginoverlap(std::function<void(Actor*)> callback)
{
	m_OnBeginOverlapCallback = callback;
}

void Actor::BindOnOutOfBounds(std::function<void(Actor*)> callback)
{
	m_OnOutOfBoundsCallback = callback;
}

void Actor::OnBeginOverlap(Actor* overlapActor)
{
	if (m_OnBeginOverlapCallback)
	{
		m_OnBeginOverlapCallback(overlapActor);
	}
}

bool Actor::HasOverlapCallback() const
{
	return static_cast<bool>(m_OnBeginOverlapCallback);
}

bool Actor::IsOutOfBounds(const sf::Vector2f& viewportSize) const
{
	sf::Vector2f location = GetLocation();
	const bool ax = location.x >= viewportSize.x + 64.0f;
	const bool bx = location.x <= -64.0f;
	const bool ay = location.y >= viewportSize.y + 64.0f;
	const bool by = location.y <= -64.0f;
	
	return ax || bx || ay || by;
}

sf::Shape* Actor::GetShape() const
{
	return m_ActorRenderShape.Get();
}

b2Body* Actor::GetBodyInstance() const
{
	return m_ActorPhysicBody.GetBodyInstance();
}

b2FixtureDef* Actor::GetFixtureDefinition() const
{
	return m_ActorPhysicBody.GetFixtureDefinition();
}

sf::Vector2f Actor::GetLocation() const
{
	if (m_ActorPhysicBody.GetBodyInstance())
	{
		b2Vec2 pos = m_ActorPhysicBody.GetBodyInstance()->GetPosition();
		return sf::Vector2f(pos.x * 32.0f, pos.y * 32.0f);
	}
	return sf::Vector2f(0, 0);
}

void Actor::SetInitTransform(const sf::Vector2f& location, float rotation)
{
	m_ActorPhysicBody.SetInitTransform(b2Vec2(location.x / 32.0f, location.y / 32.0f), rotation);
}

void Actor::ResetToInitTransform()
{
	m_ActorPhysicBody.ResetToInitTransform();
}

void Actor::Activate()
{
	m_IsActive = true;
	m_ActorPhysicBody.SetActive(true);
}

void Actor::MakeInactive()
{
	m_IsActive = false;
	m_ActorPhysicBody.SetActive(false);
}

bool Actor::IsActive() const
{
	return m_IsActive;
}

const std::string& Actor::GetObjectName() const
{
	return m_ObjectName;
}

Application* Actor::GetPackage() const
{
	return m_Package;
}

bool Actor::IsDynamic() const 
{ 
	return m_IsDynamicObject; 
}