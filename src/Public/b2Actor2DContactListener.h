#pragma once
#include "Box2DHelper.h"
#include <box2d/box2d.h>
#include <vector>
#include <functional>

class b2Actor2D;

struct FContactEvent
{
	b2Actor2D* ActorA;
	b2Actor2D* ActorB;
	bool bIsBeginContact;
};

class b2Actor2DContactListener
{
public:
	b2Actor2DContactListener();
	~b2Actor2DContactListener();

	// Process contact events from Box2D 3.x
	void ProcessContactEvents(b2WorldId worldId);

private:
	std::vector<FContactEvent> ContactEvents;
};

