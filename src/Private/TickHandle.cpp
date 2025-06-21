#include <box2d/box2d.h>
#include "TickHandle.h"
#include "Application.h"
#include "Box2DHelper.h"


FTickHandle::FTickHandle()
{
	FixedUpdateClock.restart();
}

FTickHandle::~FTickHandle()
{
	LOG("Destructing TickHandle.\n");
}

bool FTickHandle::BindApplication(Application* Object)
{
	ContextObject = Object;
	return true;
}

void FTickHandle::BeginTick()
{
	while (AppWindow.isOpen())
	{
		Tick();
	}
}

void FTickHandle::Tick()
{
	// Fixed time step for physics simulation
	const float DeltaTime = DELTA_TIME_STEP;
	
	// Step the physics world
	if (ContextObject && Box2DHelper::IsValid(ContextObject->GetWorld()))
	{
		Box2DHelper::WorldStep(ContextObject->GetWorld(), DeltaTime);
	}
	
	// Update elapsed time
	ElapsedTime += DeltaTime;
	TimeElapsedSinceLastFrame = DeltaTime;
	
	// Call the application's tick function
	if (ContextObject)
	{
		ContextObject->Tick(DeltaTime);
	}
}

void FTickHandle::EndTick()
{
	// Remove context, but do not delete as the application would delete else where.
	ContextObject = nullptr;
}
