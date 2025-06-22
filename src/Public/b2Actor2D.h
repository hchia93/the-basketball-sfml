#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "Defines.h"
#include "Interface.h"

static const float PIXEL_PER_METER = 32.0f;
class Application;

enum class EActorShapeType
{
	Circle,
	Rectangle,
	Convex
};

enum class Eb2ShapeType
{
	Polygon,
	Circle,
	Edge,
	Chain
};

struct FShapeCollection
{
	std::unique_ptr<sf::CircleShape> m_CircleShape;
	std::unique_ptr<sf::RectangleShape> m_RectangleShape;
	std::unique_ptr<sf::ConvexShape> m_ConvexShape;
	EActorShapeType m_ShapeType;

	sf::Shape* Get()
	{
		switch (m_ShapeType)
		{
			case EActorShapeType::Rectangle:
				return m_RectangleShape.get();
			case EActorShapeType::Circle:
				return m_CircleShape.get();
			case EActorShapeType::Convex:
				return m_ConvexShape.get();
		}
		return nullptr;
	}
};

struct Fb2ActorSpawnParam
{
	Application* m_Package;
	b2World* m_WorldContext;
	std::string m_Name;
	EActorShapeType m_ShapeType;
	Eb2ShapeType m_BodyType;
	sf::Vector2f m_Size;
	sf::Vector2f m_Location;
	float m_Rotation;
	bool m_bIsDynamicBody;
	bool m_bGenerateOverlaps;
	bool m_bAutoActivate;
};

class b2Actor2D : public ITickable
{
public:
	b2Actor2D(Application* package, b2World* worldContext, const std::string name, const EActorShapeType shapeType, const Eb2ShapeType bodyType, sf::Vector2f size = sf::Vector2f(1,1), sf::Vector2f location = sf::Vector2f(0,0), const float rotation = 0.0f, const bool bIsDynamicBody = false, const bool bGenerateOverlaps = false, const bool bAutoActivate = true);
	b2Actor2D(const Fb2ActorSpawnParam spawnParam);
	~b2Actor2D();

	virtual void Tick() override;
	std::string GetObjectName() const;
	sf::Shape* GetShape();
	b2FixtureDef* GetFixtureDefinition();
	b2Body* GetBodyInstance();
	Application* GetPackage() const;
	bool IsDynamic() const;
	bool IsActive() const;

	const sf::Vector2f& GetLocation();
	void SetInitTransform(sf::Vector2f location, float rotation);
	void SetInitLocation(b2Vec2 location);
	void SetInitLocation(sf::Vector2f location);
	void SetInitRotation(float rotation);
	void ResetToInitTransform();

	void BeginOverlap(b2Actor2D* overlappedActor);
	void EndOverlap(b2Actor2D* overlappedActor);
	void BindOnBeginoverlap(void (*callback)(b2Actor2D* overlappedActor));
	void BindOnEndOverlap(void (*callback)(b2Actor2D* overlappedActor));
	void BindOnTick(void (*tickFunction)(b2Actor2D* actor));

	void Activate();
	void MakeInactive();

	/** Utility*/
	static b2Vec2 Tob2Vec2Location(sf::Vector2f location);

	std::unique_ptr<sf::RectangleShape> m_DebugForward;

private:
	void Construct(Application* package, b2World* worldContext, const std::string name, const EActorShapeType shapeType, const Eb2ShapeType bodyType, sf::Vector2f size = sf::Vector2f(1, 1), sf::Vector2f location = sf::Vector2f(0, 0), const float rotation = 0.0f, const bool bIsDynamicBody = false, const bool bGenerateOverlaps = false, const bool bAutoActivate = true);
	void MakeShapeInstance(const EActorShapeType shapeType);
	void SetShapeProperties(const EActorShapeType shapeType, sf::Vector2f size);
	void MakeB2ShapeInstance(const Eb2ShapeType bodyType);
	void SetB2ShapeProperties(const Eb2ShapeType bodyType, sf::Vector2f size);

	void(*m_OnBeginOverlapCallback)(b2Actor2D* overlappedActor) = nullptr;
	void(*m_OnEndOverlapCallback)(b2Actor2D* overlappedActor) = nullptr;
	void(*m_TickCallback)(b2Actor2D* actor) = nullptr;

	std::string m_ObjectName;

	/** Internally contains unique ptr to assigned shape. (Try std::variant)*/
	FShapeCollection m_ObjectShapes;	// Act like display component

	/** Box2D Components*/
	b2Body* m_BodyInstance;
	std::unique_ptr<b2BodyDef> m_BodyDefinition;
	std::unique_ptr<b2Shape> m_BodyShape; // Act as collision component
	std::unique_ptr<b2FixtureDef> m_FixtureDefinition;
	Eb2ShapeType m_CollisionType;

	/////////////////////////////////
	//		Cache
	/////////////////////////////////

	/** Reference only, doesn't own object.*/
	Application* m_Package;

	/** Reference only, doesn't own object.*/
	sf::Shape* m_ObjectShapeCache;

	b2Vec2 m_InitialPosition;
	float m_InitialRotation;

	/////////////////////////////////
	//		Flags
	/////////////////////////////////

	bool m_bIsActive = false;
	bool m_bGenerateOverlaps = false;
	bool m_bIsDynamicObject = false;
};