#pragma once
#include <Box2D/Box2D.h>
#include "Actor.h"

class ActorContactListener : public b2ContactListener
{
public:
	ActorContactListener();
	~ActorContactListener();

	virtual void BeginContact(b2Contact* Contact);
	virtual void EndContact(b2Contact* Contact);
};

