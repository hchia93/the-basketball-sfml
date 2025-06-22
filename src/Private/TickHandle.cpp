#include "TickHandle.h"
#include "Application.h"
#include "box2d/box2d.h"

FTickHandle::~FTickHandle()
{
	LOG("Destructing TickHandle.\n");
}

bool FTickHandle::BindApplication(Application* object)
{
	m_ContextObject = object;
	return m_ContextObject != nullptr;
}

void FTickHandle::BeginTick()
{
	Tick();
}

void FTickHandle::Tick()
{
	if (!m_ContextObject)
	{
		return;
	}

	if (!m_ContextObject->GetWorld())
	{
		return;
	}

	m_TimeElapsedSinceLastFrame += m_FixedUpdateClock.restart().asSeconds();
	if (m_TimeElapsedSinceLastFrame >= DELTA_TIME_STEP)
	{
		// Step is used to update physics position/rotation
		m_ContextObject->GetWorld()->Step(
			DELTA_TIME_STEP, //update frequency
			8, //velocityIterations
			3 //positionIterations  
		);

		m_ContextObject->Tick(DELTA_TIME_STEP);
		m_TimeElapsedSinceLastFrame -= DELTA_TIME_STEP;

		m_ElapsedTime += DELTA_TIME_STEP;
	}
}

void FTickHandle::EndTick()
{
	// Remove context, but do not delete as the application would delete else where.
	m_ContextObject = nullptr;
}

float FTickHandle::GetElapsedTime() const
{
	return m_ElapsedTime;
}

void FTickHandle::ClearTimer()
{
	m_ElapsedTime = 0.0f;
}
