#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/AABB3.hpp"

class Game;
class World;

enum class PhysicsMode
{
	WALKING, 
	FLYING,  
	NOCLIP   
};

class Entity
{
public:
	Entity(Game* owner, Vec3 const& startPos);
	virtual ~Entity();

	virtual void Update(float deltaSeconds);
	virtual void Render() const;

	// Physics
	void UpdatePhysics(float deltaSeconds);
	void UpdateWalkingPhysics(float deltaSeconds);
	void UpdateFlyingPhysics(float deltaSeconds);
	void UpdateNoClipPhysics(float deltaSeconds);
	
	// Movement - 使用PhysicsUtils中的常量
	void AddForce(Vec3 const& force);
	void AddImpulse(Vec3 const& impulse);
	void MoveInDirection(Vec3 const& direction, float deltaSeconds);
	void Jump();
	
	// Collision - 使用PhysicsUtils中的辅助函数
	void ResolveWorldCollisions(float deltaSeconds);
	bool CheckGroundStatus();
	bool IsCollidingWithWorld() const;
	void PushOutOfBlocks();
	void PushOutOfBlocksX();
	void PushOutOfBlocksY();
	void PushOutOfBlocksZ();
	
	// Getters
	Vec3 GetPosition() const { return m_position; }
	Vec3 GetVelocity() const { return m_velocity; }
	Vec3 GetEyePosition() const;
	AABB3 GetPhysicsBounds() const;
	Vec3 GetForwardVector() const;
	Vec3 GetLeftVector() const;
	PhysicsMode GetPhysicsMode() const { return m_physicsMode; }
	bool IsOnGround() const { return m_isOnGround; }
	
	// Setters
	void SetPosition(Vec3 const& position) { m_position = position; }
	void SetVelocity(Vec3 const& velocity) { m_velocity = velocity; }
	void SetOrientation(EulerAngles const& orientation) { m_orientation = orientation; }
	void SetPhysicsMode(PhysicsMode mode) { m_physicsMode = mode; }

public:
	// Core properties
	Game* m_game = nullptr;
	Vec3 m_position;
	Vec3 m_velocity;
	EulerAngles m_orientation;
	
	// Physics body - 使用PhysicsUtils.h中的常量
	AABB3 m_physicsBody;  // Local space AABB
	float m_physicsHeight = 1.80f;     // PLAYER_HEIGHT from PhysicsUtils
	float m_physicsRadius = 0.30f;     // PLAYER_WIDTH / 2
	float m_eyeHeight = 1.62f;         // PLAYER_EYE_HEIGHT from PhysicsUtils
	
	// Physics state
	PhysicsMode m_physicsMode = PhysicsMode::FLYING;
	bool m_isOnGround = false;
	
	// Physics constants - 从PhysicsUtils.h导入
	float m_walkSpeed = 4.3f;          // WALK_SPEED
	float m_runSpeed = 5.6f;           // RUN_SPEED  
	float m_flySpeed = 10.9f;          // FLY_SPEED
	float m_jumpSpeed = 8.0f;          // JUMP_VELOCITY
	float m_acceleration = 40.0f;      // 加速度
	float m_groundFriction = 15.0f;    // 地面摩擦
	float m_airFriction = 2.0f;        // 空中摩擦
	
	// Rendering
	bool m_isVisible = true;
};