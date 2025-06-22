#pragma once

// Forward declarations
class Actor;

namespace GameLogic
{
	// Actor behavior functions
	void PivotTick(Actor* actor);
	void WheelTick(Actor* actor);
	void BallTick(Actor* actor);
	void SensorOverlap(Actor* overlapActor);
} 