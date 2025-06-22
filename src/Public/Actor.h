#pragma once
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <functional>
#include "Defines.h"
#include "Interface.h"
#include "ActorRenderComponent.h"
#include "ActorPhysicComponent.h"

static const float PIXEL_PER_METER = 32.0f;
class Application;

struct FActorSpawnParam
{
    Application* package;
    b2World* worldContext;
    std::string name;
    EActorShapeType shapeType;
    Eb2ShapeType bodyType;
    sf::Vector2f size;
    sf::Vector2f location;
    float rotation;
    bool isDynamicBody;
    bool generateOverlaps;
    bool autoActivate;
};

class Actor : public ITickable
{
public:
    // Constructors & Destructor
    Actor(Application* package, b2World* worldContext, const std::string& name, EActorShapeType shapeType, Eb2ShapeType bodyType, const sf::Vector2f& size, const sf::Vector2f& location, float rotation, bool isDynamicBody, bool generateOverlaps, bool autoActivate);
    Actor(const FActorSpawnParam& spawnParam);
    ~Actor();

    // Core Functionality
    virtual void Tick() override;
    void Activate();
    void MakeInactive();
    void ResetToInitTransform();

    // Getters
    const std::string& GetObjectName() const;
    sf::Shape* GetShape() const;
    b2FixtureDef* GetFixtureDefinition() const;
    b2Body* GetBodyInstance() const;
    Application* GetPackage() const;
    bool IsDynamic() const;
    bool IsActive() const;
    sf::Vector2f GetLocation() const;

    // Transform Operations
    void SetInitTransform(const sf::Vector2f& location, float rotation);

    // Collision & Overlap
    void OnBeginOverlap(Actor* overlapActor);
    void BindOnBeginoverlap(std::function<void(Actor*)> callback);
    void BindOnTick(std::function<void(Actor*)> callback);
    void BindOnOutOfBounds(std::function<void(Actor*)> callback);
    bool HasOverlapCallback() const;
    bool IsOutOfBounds(const sf::Vector2f& viewportSize) const;

    // Public Members
    std::unique_ptr<sf::RectangleShape> m_DebugForward;

private:
    // Callbacks
    std::function<void(Actor*)> m_OnBeginOverlapCallback;
    std::function<void(Actor*)> m_OnTickCallback;
    std::function<void(Actor*)> m_OnOutOfBoundsCallback;

    // Core Data
    std::string m_ObjectName;
    ActorRenderComponent m_ActorRenderShape;
    ActorPhysicComponent m_ActorPhysicBody;

    // References
    Application* m_Package;

    // State Flags
    bool m_IsActive;
    bool m_IsDynamicObject;
};