#include "ActorRenderComponent.h"

void ActorRenderComponent::CreateShape(EActorShapeType shapeType)
{
    m_ShapeType = shapeType;
    
    switch (shapeType)
    {
        case EActorShapeType::Rectangle:
            m_Shape = std::make_unique<sf::RectangleShape>();
            break;
        case EActorShapeType::Circle:
            m_Shape = std::make_unique<sf::CircleShape>();
            break;
        case EActorShapeType::Convex:
            m_Shape = std::make_unique<sf::ConvexShape>();
            break;
    }
}

sf::Shape* ActorRenderComponent::Get() const
{
    return m_Shape.get();
}

EActorShapeType ActorRenderComponent::GetShapeType() const
{
    return m_ShapeType;
}

void ActorRenderComponent::SetShapeType(EActorShapeType shapeType)
{
    m_ShapeType = shapeType;
} 