#include "GameLogic.h"
#include "Actor.h"
#include "Application.h"
#include <Box2D/Box2D.h>

namespace GameLogic
{
	void PivotTick(Actor* actor)
	{
		if (!actor)
		{
			return;
		}

		const float deltaY = 3.0f * cosf(actor->GetPackage()->GetElapsedTime()) / 32.0f;
		b2Vec2 location = actor->GetBodyInstance()->GetPosition() + b2Vec2(0, deltaY);
		actor->GetBodyInstance()->SetTransform(location, actor->GetBodyInstance()->GetAngle());
	}

	void WheelTick(Actor* actor)
	{
		if (!actor)
		{
			return;
		}

		b2Vec2 pivotLocation = actor->GetPackage()->GetPivotCache()->GetBodyInstance()->GetPosition();
		actor->GetBodyInstance()->SetTransform(pivotLocation, actor->GetBodyInstance()->GetAngle());
	}

	void BallTick(Actor* actor)
	{
		if (!actor)
		{
			return;
		}

		if (!actor->GetPackage()->GetGameState().GetIsGameStarted())
		{
			return;
		}

		// Check if ball is out of bounds
		sf::Vector2f viewportSize(actor->GetPackage()->GetAppWindowData().m_Width, actor->GetPackage()->GetAppWindowData().m_Height);
		if (actor->IsOutOfBounds(viewportSize))
		{
			actor->MakeInactive();
		}
	}

	void SensorOverlap(Actor* overlapActor)
	{
		if (!overlapActor->GetPackage()->GetGameState().GetIsGameStarted())
		{
			return;
		}

		if (overlapActor->GetObjectName() == "Ball")
		{
			overlapActor->GetPackage()->GetGameState().ScoreBall();
		}
	}
} 