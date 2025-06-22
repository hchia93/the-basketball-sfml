#include "b2Actor2DContactListener.h"
#include "b2Actor2D.h"


b2Actor2DContactListener::b2Actor2DContactListener()
{
}


b2Actor2DContactListener::~b2Actor2DContactListener()
{
}



void b2Actor2DContactListener::BeginContact(b2Contact* contact)
{
	void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();

	if (bodyUserDataA && bodyUserDataB)
	{
		static_cast<b2Actor2D*>(bodyUserDataA)->BeginOverlap(static_cast<b2Actor2D*>(bodyUserDataB));
		static_cast<b2Actor2D*>(bodyUserDataB)->BeginOverlap(static_cast<b2Actor2D*>(bodyUserDataA));
	}
}

void b2Actor2DContactListener::EndContact(b2Contact* contact)
{
	void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();

	if (bodyUserDataA && bodyUserDataB)
	{
		static_cast<b2Actor2D*>(bodyUserDataA)->EndOverlap(static_cast<b2Actor2D*>(bodyUserDataB));
		static_cast<b2Actor2D*>(bodyUserDataB)->EndOverlap(static_cast<b2Actor2D*>(bodyUserDataA));
	}
}
