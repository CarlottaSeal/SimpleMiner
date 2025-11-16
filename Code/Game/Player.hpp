#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"

class Game;
class Camera;

class Player
{
public:
	Player(Game* owner);
	~Player();

	void Update(float deltaSeconds);
	void Render() const;

	Vec3 GetForwardVectorDueToOrientation() const;
	Vec3 GetLeftVectorDueToOrientation() const;
	Mat44 GetModelToWorldTransform() const;

public:
	Vec3 m_originV;
	float m_originYaw;
	float m_originPitch;
	float m_originRoll;

	Camera m_worldCamera;

	Game* m_game = nullptr;
	Vec3 m_position;
	EulerAngles m_orientation;
	EulerAngles m_angularVelocity;
	Vec3 m_velocity;
};