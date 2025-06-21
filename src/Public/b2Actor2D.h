#pragma once
#include <SFML\Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Defines.h"
#include "Interface.h"

static const float PIXEL_PER_METER = 32.0f;
class Application;

enum EActorShapeType
{
	EST_Circle,
	EST_Rectangle,
	EST_Convex,
};

enum Eb2ShapeType
{
	ECT_Polygon,
	ECT_Circle,
	ECT_Edge,
	ECT_Chain,
};

struct FShapeCollection
{
	std::unique_ptr<SFML::CircleShape> CircleShape;
	std::unique_ptr<SFML::RectangleShape> RectangleShape;
	std::unique_ptr<SFML::ConvexShape> ConvexShape;
	EActorShapeType ShapeType;

	SFML::Shape* Get()
	{
		switch (ShapeType)
		{
			case EActorShapeType::EST_Rectangle:	return RectangleShape.get();
			case EActorShapeType::EST_Circle:		return CircleShape.get();
			case EActorShapeType::EST_Convex:		return ConvexShape.get();	
		}
		return nullptr;
	}
};

struct Fb2ActorSpawnParam
{
	Application* Package;
	b2World* WorldContext;
	std::string Name;
	EActorShapeType ShapeType;
	Eb2ShapeType BodyType;
	SFML::Vector2f Size; 
	SFML::Vector2f Location;
	float Rotation; 
	bool bIsDynamicBody; 
	bool bGenerateOverlaps;
	bool bAutoActivate;
};

class b2Actor2D : public ITickable
{
public:

	b2Actor2D(Application* Package, b2World* WorldContext, const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, SFML::Vector2f Size = SFML::Vector2f(1,1), SFML::Vector2f Location = SFML::Vector2f(0,0), const float Rotation = 0.0f, const bool bIsDynamicBody = false, const bool bGenerateOverlaps = false, const bool bAutoActivate = true);
	b2Actor2D(const Fb2ActorSpawnParam SpawnParam);
	~b2Actor2D();

	virtual void Tick() override;
	std::string		GetObjectName()			const	{ return ObjectName;  }
	SFML::Shape*	GetShape()						{ return ObjectShapes.Get(); }
	b2FixtureDef*	GetFixtureDefinition()			{ if (FixtureDefinition) return FixtureDefinition.get(); else LOG("No BodyFixture"); return nullptr; }
	
	b2Body*			GetBodyInstance()				{ if (BodyInstance) return BodyInstance; else LOG("No BodyInstance"); return nullptr; }
	Application*	GetPackage()			const	{ return Package; }
	bool			IsDynamic()				const	{ return bIsDynamicObject; }
	bool			IsActive()				const	{ return bIsActive; }

	const SFML::Vector2f&	GetLocation()			{ return ObjectShapes.Get()->getPosition(); }
	void SetInitTransform(SFML::Vector2f Location, float Rotation)	{ SetInitLocation(Location); SetInitRotation(Rotation); }
	void SetInitLocation(b2Vec2 Location)							{ InitialPosition = Location; }
	void SetInitLocation(SFML::Vector2f Location)					{ InitialPosition = b2Actor2D::Tob2Vec2Location(Location); }
	void SetInitRotation(float Rotation)							{ InitialRotation = Rotation; }
	void ResetToInitTransform();

	void BeginOverlap(b2Actor2D* OverlappedActor);
	void EndOverlap(b2Actor2D* OverlappedActor);
	void BindOnBeginoverlap(void (*Callback)(b2Actor2D* OverlappedActor));
	void BindOnEndOverlap(void (*Callback)(b2Actor2D* OverlappedActor));
	void BindOnTick(void(*TickFunction)(b2Actor2D* Actor));

	void Activate();
	void MakeInactive();

	/** Utility*/
	static b2Vec2 Tob2Vec2Location(SFML::Vector2f Location) { return b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER); };
	
	std::unique_ptr<SFML::RectangleShape> DebugForward;

private:

	void Construct(Application* Package, b2World* WorldContext, const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, SFML::Vector2f Size = SFML::Vector2f(1, 1), SFML::Vector2f Location = SFML::Vector2f(0, 0), const float Rotation = 0.0f, const bool bIsDynamicBody = false, const bool bGenerateOverlaps = false, const bool bAutoActivate = true);
	void MakeShapeInstance(const EActorShapeType ShapeType);
	void SetShapeProperties(const EActorShapeType ShapeType, SFML::Vector2f Size);
	void MakeB2ShapeInstance(const Eb2ShapeType BodyType);
	void SetB2ShapeProperties(const Eb2ShapeType BodyType, SFML::Vector2f Size);

	void(*OnBeginOverlapCallback)(b2Actor2D* OverlappedActor) = 0;
	void(*OnEndOverlapCallback)(b2Actor2D* OverlappedActor) = 0;
	void(*TickCallback)(b2Actor2D* Actor) = 0;

	std::string ObjectName;

	/** Internally contains unique ptr to assigned shape. (Try std::variant)*/
	FShapeCollection ObjectShapes;	// Act like display component

	/** Box2D Components*/
	b2Body* BodyInstance;
	std::unique_ptr<b2BodyDef>		BodyDefinition;
	std::unique_ptr<b2Shape>		BodyShape;				// Act as collision component
	std::unique_ptr<b2FixtureDef>	FixtureDefinition;
	Eb2ShapeType CollisionType;

	/////////////////////////////////
	//		Cache
	/////////////////////////////////

	/** Reference only, doesn't own object.*/
	Application* Package;

	/** Reference only, doesn't own object.*/
	SFML::Shape* ObjectShapeCache;

	b2Vec2 InitialPosition;
	float InitialRotation;

	/////////////////////////////////
	//		Flags
	/////////////////////////////////

	bool bIsActive = false;
	bool bGenerateOverlaps = false;
	bool bIsDynamicObject = false;
};