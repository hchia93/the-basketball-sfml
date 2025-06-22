#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Defines.h"

class ActorRenderComponent
{
public:
    ActorRenderComponent() = default;
    
    // Factory method to create the appropriate shape
    void CreateShape(EActorShapeType shapeType);
    
    // Get the shape (no longer needs switch statement)
    sf::Shape* Get() const;
    
    // Get the shape type
    EActorShapeType GetShapeType() const;
    
    // Set the shape type
    void SetShapeType(EActorShapeType shapeType);

private:
    std::unique_ptr<sf::Shape> m_Shape;
    EActorShapeType m_ShapeType;
}; 