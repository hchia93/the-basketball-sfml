#pragma once
#include <box2d/box2d.h>


class b2Actor2DContactListener : public b2ContactListener
{
public:
	b2Actor2DContactListener();
	~b2Actor2DContactListener();

	virtual void BeginContact(b2Contact* contact) override;
	virtual void EndContact(b2Contact* contact) override;
};

