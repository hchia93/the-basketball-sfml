#pragma once
#include <Box2D/Box2D.h>


class b2Actor2DContactListener : public b2ContactListener
{
public:
	b2Actor2DContactListener();
	~b2Actor2DContactListener();

	virtual void BeginContact(b2Contact* Contact);
	virtual void EndContact(b2Contact* Contact);
};

