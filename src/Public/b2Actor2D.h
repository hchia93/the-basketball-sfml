#pragma once
#include <SFML\Graphics.hpp>
#include <box2d/box2d.h>
#include <memory>
#include <string>
#include "Defines.h"
#include "Interface.h"
#include "Box2DHelper.h"

static const float PIXEL_PER_METER = 32.0f;
class Application;

enum EActorShapeType
{
	Circle,
	Rectangle,
	Convex,
};

enum Eb2ShapeType
{
	Polygon,
	Circle,
	Edge,
	Chain,
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
			case EActorShapeType::Rectangle: return m_RectangleShape.get();
			case EActorShapeType::Circle: return m_CircleShape.get();
			case EActorShapeType::Convex: return m_ConvexShape.get();
		}
		return nullptr;
	}
};

struct Fb2ActorSpawnParam
{
	Application* Package;
	b2WorldId WorldContext;
	std::string Name;
	EActorShapeType ShapeType;
	Eb2ShapeType BodyType;
	sf::Vector2f Size; 
	sf::Vector2f Location;
	float Rotation; 
	bool bIsDynamicBody; 
	bool bGenerateOverlaps;
	bool bAutoActivate;
};

class b2Actor2D : public ITickable
{
public:
	b2Actor2D(Application* Package, b2WorldId WorldContext, const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, sf::Vector2f Size = sf::Vector2f(1, 1), sf::Vector2f Location = sf::Vector2f(0, 0), const float Rotation = 0.0f, const bool bIsDynamicBody = false, const bool bGenerateOverlaps = false, const bool bAutoActivate = true);
	b2Actor2D(const Fb2ActorSpawnParam SpawnParam);
	~b2Actor2D();

	virtual void Tick() override;
	std::string GetObjectName() const { return m_ObjectName; }
	sf::Shape* GetShape() { return m_ObjectShapes.Get(); }
	b2ShapeDef* GetShapeDefinition() { if (m_ShapeDefinition) return m_ShapeDefinition.get(); else LOG("No ShapeDefinition"); return nullptr; }
	
	b2BodyId GetBodyInstance() { if (Box2DHelper::IsValid(m_BodyInstance)) return m_BodyInstance; else LOG("No BodyInstance"); return b2BodyId_null; }
	Application* GetPackage() const { return m_Package; }
	bool IsDynamic() const { return m_bIsDynamicObject; }
	bool IsActive() const { return m_bIsActive; }

	const sf::Vector2f& GetLocation() { return m_ObjectShapes.Get()->getPosition(); }
	void SetInitTransform(sf::Vector2f Location, float Rotation) { SetInitLocation(Location); SetInitRotation(Rotation); }
	void SetInitLocation(b2Vec2 Location) { m_InitialPosition = Location; }
	void SetInitLocation(sf::Vector2f Location) { m_InitialPosition = b2Actor2D::Tob2Vec2Location(Location); }
	void SetInitRotation(float Rotation) { m_InitialRotation = Rotation; }
	void ResetToInitTransform();

	void BeginOverlap(b2Actor2D* OverlappedActor);
	void EndOverlap(b2Actor2D* OverlappedActor);
	void BindOnBeginoverlap(void (*Callback)(b2Actor2D* OverlappedActor));
	void BindOnEndOverlap(void (*Callback)(b2Actor2D* OverlappedActor));
	void BindOnTick(void(*TickFunction)(b2Actor2D* Actor));

	void Activate();
	void MakeInactive();

	/** Utility */
	static b2Vec2 Tob2Vec2Location(sf::Vector2f Location) { return b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER); }
	
	std::unique_ptr<sf::RectangleShape> m_DebugForward;

private:
	void Construct(Application* Package, b2WorldId WorldContext, const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, sf::Vector2f Size = sf::Vector2f(1, 1), sf::Vector2f Location = sf::Vector2f(0, 0), const float Rotation = 0.0f, const bool bIsDynamicBody = false, const bool bGenerateOverlaps = false, const bool bAutoActivate = true);
	void MakeShapeInstance(const EActorShapeType ShapeType);
	void SetShapeProperties(const EActorShapeType ShapeType, sf::Vector2f Size);
	void MakeB2ShapeInstance(const Eb2ShapeType BodyType);
	void SetB2ShapeProperties(const Eb2ShapeType BodyType, sf::Vector2f Size);

	void(*m_OnBeginOverlapCallback)(b2Actor2D* OverlappedActor) = 0;
	void(*m_OnEndOverlapCallback)(b2Actor2D* OverlappedActor) = 0;
	void(*m_TickCallback)(b2Actor2D* Actor) = 0;

	std::string m_ObjectName;

	/** Internally contains unique ptr to assigned shape. (Try std::variant) */
	FShapeCollection m_ObjectShapes;	// Act like display component

	/** Box2D 3.x Components */
	b2BodyId m_BodyInstance;
	std::unique_ptr<b2BodyDef> m_BodyDefinition;
	std::unique_ptr<b2ShapeDef> m_ShapeDefinition;
	Eb2ShapeType m_CollisionType;

	/////////////////////////////////
	//		Cache
	/////////////////////////////////

	/** Reference only, doesn't own object. */
	Application* m_Package;

	/** Reference only, doesn't own object. */
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