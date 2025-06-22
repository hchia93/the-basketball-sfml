#pragma once
#include <Box2D/Box2D.h>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "Defines.h"

class ActorPhysicComponent
{
public:
    ActorPhysicComponent();
    ~ActorPhysicComponent();

    // No copy, allow move
    ActorPhysicComponent(const ActorPhysicComponent&) = delete;
    ActorPhysicComponent& operator=(const ActorPhysicComponent&) = delete;
    ActorPhysicComponent(ActorPhysicComponent&&) noexcept;
    ActorPhysicComponent& operator=(ActorPhysicComponent&&) noexcept;

    void Construct(b2World* worldContext, Eb2ShapeType bodyType, const sf::Vector2f& size, const sf::Vector2f& location, float rotation, bool isDynamicBody, bool generateOverlaps);
    void Destroy(b2World* worldContext);

    // Accessors
    b2Body* GetBodyInstance() const;
    b2FixtureDef* GetFixtureDefinition() const;
    b2Shape* GetBodyShape() const;
    Eb2ShapeType GetCollisionType() const;

    // Transform
    void SetTransform(const b2Vec2& position, float angle);
    void SetActive(bool active);
    void SetAwake(bool awake);
    b2Vec2 GetPosition() const;
    float GetAngle() const;

    // Utility
    void SetInitTransform(const b2Vec2& position, float angle);
    void ResetToInitTransform();

private:
    b2Body* m_BodyInstance = nullptr;
    std::unique_ptr<b2BodyDef> m_BodyDefinition;
    std::unique_ptr<b2Shape> m_BodyShape;
    std::unique_ptr<b2FixtureDef> m_FixtureDefinition;
    Eb2ShapeType m_CollisionType;
    b2Vec2 m_InitialPosition;
    float m_InitialRotation = 0.0f;
    bool m_GenerateOverlaps = false;
    bool m_IsDynamicObject = false;
}; 