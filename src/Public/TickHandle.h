#pragma once
#include <SFML/System.hpp>
#include <ctime>
#include "Defines.h"
#include "Interface.h"

class Application;

class FTickHandle : public ITickable
{
public:
	FTickHandle();
	~FTickHandle();

	/* 
	 * Binding instance of application into tick handle and call tick here with delta step. 
	 * Ideally, should forward dilation to object instance, but the main tick should tick at normal delta step.
	 */
	bool BindApplication(Application* Object);

	/* Wrapper function to instruct Tick(). */
	void BeginTick();

	virtual void Tick() override;

	/* Handle Finalization after last Tick called. */
	void EndTick();

	float GetElapsedTime() const { return m_ElapsedTime; }
	
	void ClearTimer() { m_ElapsedTime = 0.0f; }

private:
	float m_ElapsedTime = 0.0f;
	float m_TimeElapsedSinceLastFrame = 0.0f;
	SFML::Clock m_FixedUpdateClock;
	Application* m_ContextObject;
};

