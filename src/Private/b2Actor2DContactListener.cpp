#include "b2Actor2DContactListener.h"
#include "b2Actor2D.h"


b2Actor2DContactListener::b2Actor2DContactListener()
{
}


b2Actor2DContactListener::~b2Actor2DContactListener()
{
}

void b2Actor2DContactListener::ProcessContactEvents(b2WorldId worldId)
{
	b2ContactEvents contactEvents = Box2DHelper::GetContactEvents(worldId);
	
	// Process begin contact events
	for (int i = 0; i < contactEvents.beginCount; ++i)
	{
		b2ContactBeginTouchEvent& event = contactEvents.beginEvents[i];
		void* BodyUserDataA = Box2DHelper::GetUserData(event.bodyIdA);
		void* BodyUserDataB = Box2DHelper::GetUserData(event.bodyIdB);

		if (BodyUserDataA)
			static_cast<b2Actor2D*>(BodyUserDataA)->BeginOverlap(static_cast<b2Actor2D*>(BodyUserDataB));

		if (BodyUserDataB)
			static_cast<b2Actor2D*>(BodyUserDataB)->BeginOverlap(static_cast<b2Actor2D*>(BodyUserDataA));
	}

	// Process end contact events
	for (int i = 0; i < contactEvents.endCount; ++i)
	{
		b2ContactEndTouchEvent& event = contactEvents.endEvents[i];
		void* BodyUserDataA = Box2DHelper::GetUserData(event.bodyIdA);
		void* BodyUserDataB = Box2DHelper::GetUserData(event.bodyIdB);

		if (BodyUserDataA)
			static_cast<b2Actor2D*>(BodyUserDataA)->EndOverlap(static_cast<b2Actor2D*>(BodyUserDataB));

		if (BodyUserDataB)
			static_cast<b2Actor2D*>(BodyUserDataB)->EndOverlap(static_cast<b2Actor2D*>(BodyUserDataA));
	}
}
