#include "ActorContactListener.h"
#include "Actor.h"

ActorContactListener::ActorContactListener()
{
}

ActorContactListener::~ActorContactListener()
{
}

void ActorContactListener::BeginContact(b2Contact* Contact)
{
	void* BodyUserDataA = reinterpret_cast<void*>(Contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	void* BodyUserDataB = reinterpret_cast<void*>(Contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	Actor* actorA = static_cast<Actor*>(BodyUserDataA);
	Actor* actorB = static_cast<Actor*>(BodyUserDataB);

	// Call OnBeginOverlap with the other actor as parameter
	if (actorA && actorA->HasOverlapCallback())
		actorA->OnBeginOverlap(actorB);

	if (actorB && actorB->HasOverlapCallback())
		actorB->OnBeginOverlap(actorA);
}

void ActorContactListener::EndContact(b2Contact* Contact)
{
	// End contact handling can be added here if needed
}
