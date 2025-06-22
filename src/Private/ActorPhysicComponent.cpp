#include "ActorPhysicComponent.h"

ActorPhysicComponent::ActorPhysicComponent() = default;
ActorPhysicComponent::~ActorPhysicComponent() = default;
ActorPhysicComponent::ActorPhysicComponent(ActorPhysicComponent&&) noexcept = default;
ActorPhysicComponent& ActorPhysicComponent::operator=(ActorPhysicComponent&&) noexcept = default;

void ActorPhysicComponent::Construct(b2World* worldContext, Eb2ShapeType bodyType, const sf::Vector2f& size, const sf::Vector2f& location, float rotation, bool isDynamicBody, bool generateOverlaps)
{
    m_CollisionType = bodyType;
    m_GenerateOverlaps = generateOverlaps;
    m_IsDynamicObject = isDynamicBody;

    m_BodyDefinition = std::make_unique<b2BodyDef>();
    m_BodyDefinition->position = b2Vec2(location.x / 32.0f, location.y / 32.0f);
    m_BodyDefinition->type = isDynamicBody ? b2_dynamicBody : b2_staticBody;
    m_InitialPosition = m_BodyDefinition->position;
    m_InitialRotation = rotation;

    // Create shape
    switch (bodyType) {
        case Eb2ShapeType::Chain:   m_BodyShape = std::make_unique<b2ChainShape>(); break;
        case Eb2ShapeType::Edge:    m_BodyShape = std::make_unique<b2EdgeShape>(); break;
        case Eb2ShapeType::Polygon: m_BodyShape = std::make_unique<b2PolygonShape>(); break;
        case Eb2ShapeType::Circle:  m_BodyShape = std::make_unique<b2CircleShape>(); break;
    }
    // Set shape properties
    switch (bodyType) {
        case Eb2ShapeType::Polygon:
            if (auto* p = dynamic_cast<b2PolygonShape*>(m_BodyShape.get()))
                p->SetAsBox((size.x * 0.5f) / 32.0f, (size.y * 0.5f) / 32.0f);
            break;
        case Eb2ShapeType::Circle:
            if (auto* p = dynamic_cast<b2CircleShape*>(m_BodyShape.get()))
                p->m_radius = (size.x * 0.5f) / 32.0f;
            break;
        default: break;
    }
    m_FixtureDefinition = std::make_unique<b2FixtureDef>();
    m_FixtureDefinition->shape = m_BodyShape.get();
    m_FixtureDefinition->density = 0.5f;
    m_FixtureDefinition->friction = 0.5f;
    m_FixtureDefinition->restitution = 0.5f;
    m_FixtureDefinition->isSensor = generateOverlaps;

    m_BodyInstance = worldContext->CreateBody(m_BodyDefinition.get());
    m_BodyInstance->CreateFixture(m_FixtureDefinition.get());
    // UserData assignment is left to the owner
}

void ActorPhysicComponent::Destroy(b2World* worldContext)
{
    if (m_BodyInstance && worldContext) {
        worldContext->DestroyBody(m_BodyInstance);
        m_BodyInstance = nullptr;
    }
}

b2Body* ActorPhysicComponent::GetBodyInstance() const { return m_BodyInstance; }
b2FixtureDef* ActorPhysicComponent::GetFixtureDefinition() const { return m_FixtureDefinition.get(); }
b2Shape* ActorPhysicComponent::GetBodyShape() const { return m_BodyShape.get(); }
Eb2ShapeType ActorPhysicComponent::GetCollisionType() const { return m_CollisionType; }

void ActorPhysicComponent::SetTransform(const b2Vec2& position, float angle)
{
    if (m_BodyInstance) m_BodyInstance->SetTransform(position, angle);
}
void ActorPhysicComponent::SetActive(bool active)
{
    if (m_BodyInstance) m_BodyInstance->SetEnabled(active);
}
void ActorPhysicComponent::SetAwake(bool awake)
{
    if (m_BodyInstance) m_BodyInstance->SetAwake(awake);
}
b2Vec2 ActorPhysicComponent::GetPosition() const
{
    return m_BodyInstance ? m_BodyInstance->GetPosition() : b2Vec2(0,0);
}
float ActorPhysicComponent::GetAngle() const
{
    return m_BodyInstance ? m_BodyInstance->GetAngle() : 0.0f;
}
void ActorPhysicComponent::SetInitTransform(const b2Vec2& position, float angle)
{
    m_InitialPosition = position;
    m_InitialRotation = angle;
}
void ActorPhysicComponent::ResetToInitTransform()
{
    if (m_BodyInstance)
        m_BodyInstance->SetTransform(m_InitialPosition, m_InitialRotation);
} 