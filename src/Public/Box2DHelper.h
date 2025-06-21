#pragma once
#include <box2d/box2d.h>
#include <memory>
#include <vector>

// A helper namespace to provide backward compatibility from Box2D 3.0+ to Box2D 2.4.1

namespace Box2DHelper
{
    // World management
    inline b2WorldId CreateWorld(const b2Vec2& gravity)
    {
        b2WorldDef worldDef;
        worldDef.gravity = gravity;
        return b2CreateWorld(&worldDef);
    }

    inline void DestroyWorld(b2WorldId worldId)
    {
        b2DestroyWorld(worldId);
    }

    inline void WorldStep(b2WorldId worldId, float timeStep, int subStepCount = 4)
    {
        b2World_Step(worldId, timeStep, subStepCount);
    }

    inline void SetGravity(b2WorldId worldId, const b2Vec2& gravity)
    {
        b2World_SetGravity(worldId, gravity);
    }

    inline b2Vec2 GetGravity(b2WorldId worldId)
    {
        return b2World_GetGravity(worldId);
    }

    // Body management
    inline b2BodyId CreateBody(b2WorldId worldId, const b2BodyDef& bodyDef)
    {
        return b2CreateBody(worldId, &bodyDef);
    }

    inline void DestroyBody(b2WorldId worldId, b2BodyId bodyId)
    {
        b2World_DestroyBody(worldId, bodyId);
    }

    inline void SetTransform(b2BodyId bodyId, const b2Vec2& position, float angle)
    {
        b2Rot rotation;
        rotation.s = sinf(angle);
        rotation.c = cosf(angle);
        b2Body_SetTransform(bodyId, position, rotation);
    }

    inline b2Transform GetTransform(b2BodyId bodyId)
    {
        return b2Body_GetTransform(bodyId);
    }

    inline b2Vec2 GetPosition(b2BodyId bodyId)
    {
        return b2Body_GetPosition(bodyId);
    }

    inline float GetAngle(b2BodyId bodyId)
    {
        b2Rot rotation = b2Body_GetRotation(bodyId);
        return atan2f(rotation.s, rotation.c);
    }

    inline void SetActive(b2BodyId bodyId, bool flag)
    {
        b2Body_SetActive(bodyId, flag);
    }

    inline void SetAwake(b2BodyId bodyId, bool flag)
    {
        b2Body_SetAwake(bodyId, flag);
    }

    inline void SetUserData(b2BodyId bodyId, void* userData)
    {
        b2Body_SetUserData(bodyId, userData);
    }

    inline void* GetUserData(b2BodyId bodyId)
    {
        return b2Body_GetUserData(bodyId);
    }

    // Velocity functions
    inline b2Vec2 GetLinearVelocity(b2BodyId bodyId)
    {
        return b2Body_GetLinearVelocity(bodyId);
    }

    inline void SetLinearVelocity(b2BodyId bodyId, const b2Vec2& linearVelocity)
    {
        b2Body_SetLinearVelocity(bodyId, linearVelocity);
    }

    // Shape management
    inline b2ShapeId CreateCircleShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, float radius)
    {
        b2Circle circle;
        circle.center = b2Vec2_zero;
        circle.radius = radius;
        return b2CreateCircleShape(bodyId, &shapeDef, &circle);
    }

    inline b2ShapeId CreatePolygonShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Vec2* points, int count)
    {
        b2Polygon polygon;
        for (int i = 0; i < count && i < b2_maxPolygonVertices; ++i)
        {
            polygon.vertices[i] = points[i];
        }
        polygon.count = count;
        return b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
    }

    inline b2ShapeId CreateBoxShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, float halfWidth, float halfHeight)
    {
        b2Polygon polygon;
        b2MakeBox(&polygon, halfWidth, halfHeight);
        return b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
    }

    inline b2ShapeId CreateEdgeShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Vec2& v1, const b2Vec2& v2)
    {
        b2Segment segment;
        segment.point1 = v1;
        segment.point2 = v2;
        return b2CreateSegmentShape(bodyId, &shapeDef, &segment);
    }

    // Contact events
    inline b2ContactEvents GetContactEvents(b2WorldId worldId)
    {
        return b2World_GetContactEvents(worldId);
    }

    // Utility functions
    inline bool IsValid(b2WorldId worldId)
    {
        return b2World_IsValid(worldId);
    }

    inline bool IsValid(b2BodyId bodyId)
    {
        return b2Body_IsValid(bodyId);
    }

    inline bool IsValid(b2ShapeId shapeId)
    {
        return b2Shape_IsValid(shapeId);
    }
} 