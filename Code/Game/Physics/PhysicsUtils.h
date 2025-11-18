#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"

class World;
class Block;

//-----------------------------------------------------------------------------
// Physics Constants
//-----------------------------------------------------------------------------
constexpr float PLAYER_WIDTH = 0.6f;           // Player is 0.6 blocks wide
constexpr float PLAYER_HEIGHT = 1.8f;          // Player is 1.8 blocks tall
constexpr float PLAYER_EYE_HEIGHT = 1.62f;     // Eye level from feet
constexpr float PLAYER_CROUCH_HEIGHT = 1.5f;   // Height when crouching

constexpr float GRAVITY = -20.0f;              // Gravity acceleration (blocks/s^2)
constexpr float TERMINAL_VELOCITY = -50.0f;    // Max fall speed (blocks/s)
constexpr float JUMP_VELOCITY = 8.0f;          // Initial jump velocity
constexpr float WALK_SPEED = 4.3f;             // Walking speed (blocks/s)
constexpr float RUN_SPEED = 5.6f;              // Running speed (blocks/s)
constexpr float SWIM_SPEED = 2.0f;             // Swimming speed (blocks/s)
constexpr float FLY_SPEED = 10.9f;             // Creative fly speed (blocks/s)

constexpr float WATER_GRAVITY_MULTIPLIER = 0.2f;  // Gravity reduction in water
constexpr float WATER_DRAG = 0.8f;                // Velocity damping in water
constexpr float AIR_DRAG = 0.98f;                 // Air resistance


// Get penetration depth between two AABBs
inline Vec3 GetAABBPenetration(const AABB3& a, const AABB3& b)
{
    Vec3 penetration;
    
    // X-axis
    float leftPen = a.m_maxs.x - b.m_mins.x;
    float rightPen = b.m_maxs.x - a.m_mins.x;
    penetration.x = (leftPen < rightPen) ? leftPen : -rightPen;
    
    // Y-axis
    float backPen = a.m_maxs.y - b.m_mins.y;
    float frontPen = b.m_maxs.y - a.m_mins.y;
    penetration.y = (backPen < frontPen) ? backPen : -frontPen;
    
    // Z-axis
    float topPen = a.m_maxs.z - b.m_mins.z;
    float bottomPen = b.m_maxs.z - a.m_mins.z;
    penetration.z = (topPen < bottomPen) ? topPen : -bottomPen;
    
    return penetration;
}

// Resolve collision by pushing out along smallest penetration axis
inline Vec3 ResolveAABBCollision(const AABB3& movingBox, const AABB3& staticBox, const Vec3& velocity)
{
    Vec3 penetration = GetAABBPenetration(movingBox, staticBox);
    
    // Find axis with smallest penetration
    float minPen = fabsf(penetration.x);
    Vec3 pushOut = Vec3(penetration.x, 0, 0);
    
    if (fabsf(penetration.y) < minPen)
    {
        minPen = fabsf(penetration.y);
        pushOut = Vec3(0, penetration.y, 0);
    }
    
    if (fabsf(penetration.z) < minPen)
    {
        pushOut = Vec3(0, 0, penetration.z);
    }
    
    // Push out in direction opposite to velocity
    if (DotProduct3D(pushOut, velocity) > 0)
    {
        pushOut = -pushOut;
    }
    
    return pushOut;
}

//-----------------------------------------------------------------------------
// Step Detection
//-----------------------------------------------------------------------------

struct StepResult
{
    bool canStep = false;
    float stepHeight = 0.0f;
    Vec3 stepPosition;
};

// Check if player can step up onto a block
inline StepResult CheckStepUp(const Vec3& currentPos, const Vec3& desiredPos, 
                              float maxStepHeight, World* world)
{
    StepResult result;
    
    // Try different step heights
    for (float stepHeight = 0.1f; stepHeight <= maxStepHeight; stepHeight += 0.1f)
    {
        Vec3 stepPos = desiredPos + Vec3(0, 0, stepHeight);
        
        // Check if position is valid at this height
        AABB3 testBox(stepPos - Vec3(PLAYER_WIDTH * 0.5f, PLAYER_WIDTH * 0.5f, PLAYER_HEIGHT * 0.5f),
                     stepPos + Vec3(PLAYER_WIDTH * 0.5f, PLAYER_WIDTH * 0.5f, PLAYER_HEIGHT * 0.5f));
        
        bool collision = false;
        // Would need to check collision with world here
        
        if (!collision)
        {
            result.canStep = true;
            result.stepHeight = stepHeight;
            result.stepPosition = stepPos;
            break;
        }
    }
    
    return result;
}

//-----------------------------------------------------------------------------
// Movement Helpers
//-----------------------------------------------------------------------------

// Apply friction to horizontal velocity
inline Vec3 ApplyFriction(const Vec3& velocity, float friction, float deltaSeconds)
{
    Vec3 result = velocity;
    float speed = Vec2(velocity.x, velocity.y).GetLength();
    
    if (speed > 0.01f)
    {
        float drop = speed * friction * deltaSeconds;
        float newSpeed = fmaxf(speed - drop, 0.0f);
        float scale = newSpeed / speed;
        result.x *= scale;
        result.y *= scale;
    }
    
    return result;
}

// Apply acceleration with max speed limit
inline Vec3 Accelerate(const Vec3& velocity, const Vec3& wishDir, float wishSpeed, 
                       float accel, float deltaSeconds)
{
    float currentSpeed = DotProduct3D(velocity, wishDir);
    float addSpeed = wishSpeed - currentSpeed;
    
    if (addSpeed <= 0)
        return velocity;
    
    float accelSpeed = accel * deltaSeconds * wishSpeed;
    if (accelSpeed > addSpeed)
        accelSpeed = addSpeed;
    
    return velocity + wishDir * accelSpeed;
}

//-----------------------------------------------------------------------------
// Block State Queries
//-----------------------------------------------------------------------------

// Check if block at position is water
inline bool IsInWater(const Vec3& position, World* world)
{
    // Implementation would check if position is in water block
    return false;
}

// Check if block at position is lava
inline bool IsInLava(const Vec3& position, World* world)
{
    // Implementation would check if position is in lava block
    return false;
}

// Get fluid level at position (0.0 = no fluid, 1.0 = full block)
inline float GetFluidLevel(const Vec3& position, World* world)
{
    // Implementation would return fluid level
    return 0.0f;
}