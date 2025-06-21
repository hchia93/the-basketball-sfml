#include <Box2D/Box2D.h>
#include "TickHandle.h"
#include "Application.h"


FTickHandle::FTickHandle()
{

}

FTickHandle::~FTickHandle()
{
	LOG("Destructing TickHandle.\n");
}

bool FTickHandle::BindApplication(Application* Object)
{
	ContextObject = Object;
	return ContextObject != nullptr;
}

void FTickHandle::BeginTick()
{
	Tick();
}

void FTickHandle::Tick()
{
	if (!ContextObject) return;
	if (!ContextObject->GetWorld()) return;

	
	TimeElapsedSinceLastFrame += FixedUpdateClock.restart().asSeconds();
	if (TimeElapsedSinceLastFrame >= DELTA_TIME_STEP)
	{
		// Step is used to update physics position/rotation
		ContextObject->GetWorld()->Step(DELTA_TIME_STEP,	//update frequency
			8,												//velocityIterations
			3												//positionIterations  
		);

		ContextObject->Tick(DELTA_TIME_STEP);
		TimeElapsedSinceLastFrame -= DELTA_TIME_STEP;

		ElapsedTime += DELTA_TIME_STEP;
	}
}

void FTickHandle::EndTick()
{
	// Remove context, but do not delete as the application would delete else where.
	ContextObject = nullptr;
}
