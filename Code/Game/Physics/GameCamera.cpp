#include "GameCamera.h"
#include "Entity.h"
#include "Game/World.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Input/InputSystem.hpp"

extern RandomNumberGenerator* g_theRNG;
extern InputSystem* g_theInput;

GameCamera::GameCamera()
{
}

GameCamera::~GameCamera()
{
}

void GameCamera::Update(float deltaSeconds, Entity* entity, World* world)
{
	// Update camera shake
	if (m_isCameraShaking)
	{
		UpdateCameraShake(deltaSeconds);
	}
	
	// Update based on mode
	switch (m_cameraMode)
	{
	case GameCameraMode::FIRST_PERSON:
		UpdateFirstPerson(entity);
		break;
		
	case GameCameraMode::OVER_SHOULDER:
		UpdateOverShoulder(entity, world);
		break;
		
	case GameCameraMode::FIXED_ANGLE_TRACKING:
		UpdateFixedAngleTracking(entity);
		break;
		
	case GameCameraMode::SPECTATOR:
		UpdateSpectator(deltaSeconds);
		break;
		
	case GameCameraMode::SPECTATOR_XY:
		UpdateSpectatorXY(deltaSeconds);
		break;
		
	case GameCameraMode::INDEPENDENT:
		UpdateIndependent(entity);
		break;
	}
}

void GameCamera::UpdateFirstPerson(Entity* entity)
{
	if (!entity)
		return;
	
	// Camera at entity eye position
	Vec3 eyePos = entity->GetEyePosition();
	
	// Add camera shake
	if (m_isCameraShaking)
	{
		eyePos += m_shakeOffset;
	}
	
	SetPosition(eyePos);
	SetOrientation(entity->m_orientation);
}

void GameCamera::UpdateOverShoulder(Entity* entity, World* world)
{
	if (!entity)
		return;
	
	// Pivot point (slightly above entity position)
	Vec3 pivotPoint = entity->GetPosition() + m_overShoulderOffset;
	
	// Calculate target camera position (behind entity)
	Vec3 forward = entity->GetForwardVector();
	Vec3 targetPos = pivotPoint - forward * m_overShoulderDistance;
	targetPos.z += 0.5f; // Slightly above
	
	// Collision detection (keep camera out of walls)
	if (world && entity->GetPhysicsMode() != PhysicsMode::NOCLIP)
	{
		targetPos = ResolveThirdPersonCollision(targetPos, pivotPoint, world);
	}
	
	// Add camera shake
	if (m_isCameraShaking)
	{
		targetPos += m_shakeOffset;
	}
	
	SetPosition(targetPos);
	SetOrientation(entity->m_orientation);
}

void GameCamera::UpdateFixedAngleTracking(Entity* entity)
{
	if (!entity)
		return;
	
	// Camera at fixed angle from entity
	Vec3 entityPos = entity->GetPosition();
	
	// Calculate offset using spherical coordinates
	float yawRad = m_fixedTrackingYaw * (PI / 180.0f);
	float pitchRad = m_fixedTrackingPitch * (PI / 180.0f);
	
	float cosPitch = cosf(pitchRad);
	Vec3 offset;
	offset.x = m_fixedTrackingDistance * cosPitch * cosf(yawRad);
	offset.y = m_fixedTrackingDistance * cosPitch * sinf(yawRad);
	offset.z = m_fixedTrackingDistance * sinf(pitchRad);
	
	Vec3 cameraPos = entityPos + offset;
	
	// Add camera shake
	if (m_isCameraShaking)
	{
		cameraPos += m_shakeOffset;
	}
	
	// Look at entity
	Vec3 lookDir = entityPos - cameraPos;
	lookDir = lookDir.GetNormalized();
	
	EulerAngles lookOrientation;
	lookOrientation.m_yawDegrees = Atan2Degrees(lookDir.y, lookDir.x);
	lookOrientation.m_pitchDegrees = Atan2Degrees(-lookDir.z, 
		sqrtf(lookDir.x * lookDir.x + lookDir.y * lookDir.y));
	
	SetPosition(cameraPos);
	SetOrientation(lookOrientation);
}

void GameCamera::UpdateSpectator(float deltaSeconds)
{
	// Free camera - player dispossessed
	// Camera orientation controlled by mouse (in player code)
	
	// Check for run
	bool isRunning = g_theInput->IsKeyDown(KEYCODE_SHIFT);
	float speed = isRunning ? m_spectatorRunSpeed : m_spectatorSpeed;
	
	// Build movement direction
	Vec3 moveDir = Vec3();
	
	Vec3 forward, left, up;
	m_orientation.GetAsVectors_IFwd_JLeft_KUp(forward, left, up);
	
	// WASD movement relative to camera orientation
	if (g_theInput->IsKeyDown('W'))
		moveDir += forward;
	if (g_theInput->IsKeyDown('S'))
		moveDir -= forward;
	if (g_theInput->IsKeyDown('A'))
		moveDir += left;
	if (g_theInput->IsKeyDown('D'))
		moveDir -= left;
	if (g_theInput->IsKeyDown('Q'))
		moveDir += up;
	if (g_theInput->IsKeyDown('E'))
		moveDir -= up;
	
	// Normalize and apply
	if (moveDir.GetLengthSquared() > 0.0f)
	{
		moveDir = moveDir.GetNormalized();
		m_position += moveDir * speed * deltaSeconds;
	}
	
	// Controller support
	XboxController const& controller = g_theInput->GetController(0);
	Vec2 leftStick = controller.GetLeftStick().GetPosition();
	if (leftStick.GetLengthSquared() > 0.01f)
	{
		Vec3 controllerMove = forward * leftStick.y + left * (-leftStick.x);
		controllerMove = controllerMove.GetNormalized();
		m_position += controllerMove * speed * deltaSeconds;
	}
	
	// Vertical with triggers
	float verticalInput = controller.GetLeftTrigger() - controller.GetRightTrigger();
	if (fabsf(verticalInput) > 0.01f)
	{
		m_position += up * verticalInput * speed * deltaSeconds;
	}
	
	Vec2 mouseDelta = g_theInput->GetCursorClientDelta();
	m_orientation.m_yawDegrees -= mouseDelta.x * 0.075f;
	m_orientation.m_pitchDegrees += mouseDelta.y * 0.075f;
	
	m_orientation.m_yawDegrees -= controller.GetRightStick().GetPosition().x * 90.0f * deltaSeconds;
	m_orientation.m_pitchDegrees += controller.GetRightStick().GetPosition().y * 90.0f * deltaSeconds;
	
	m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -85.0f, 85.0f);
	m_orientation.m_yawDegrees = fmodf(m_orientation.m_yawDegrees, 360.0f);
}

void GameCamera::UpdateSpectatorXY(float deltaSeconds)
{
	bool isRunning = g_theInput->IsKeyDown(KEYCODE_SHIFT);
	float speed = isRunning ? m_spectatorRunSpeed : m_spectatorSpeed;
	
	Vec3 moveDir = Vec3();
	
	Vec3 forward, left, up;
	m_orientation.GetAsVectors_IFwd_JLeft_KUp(forward, left, up);
	

	forward.z = 0.0f;
	if (forward.GetLengthSquared() > 0.0f)
		forward = forward.GetNormalized();
	
	left.z = 0.0f;
	if (left.GetLengthSquared() > 0.0f)
		left = left.GetNormalized();
	
	// WASD movement (XY only)
	if (g_theInput->IsKeyDown('W'))
		moveDir += forward;
	if (g_theInput->IsKeyDown('S'))
		moveDir -= forward;
	if (g_theInput->IsKeyDown('A'))
		moveDir += left;
	if (g_theInput->IsKeyDown('D'))
		moveDir -= left;
	
	// Q/E for vertical
	if (g_theInput->IsKeyDown('Q'))
		moveDir.z += 1.0f;
	if (g_theInput->IsKeyDown('E'))
		moveDir.z -= 1.0f;
	
	if (moveDir.GetLengthSquared() > 0.0f)
	{
		moveDir = moveDir.GetNormalized();
		m_position += moveDir * speed * deltaSeconds;
	}
	
	// Controller
	XboxController const& controller = g_theInput->GetController(0);
	Vec2 leftStick = controller.GetLeftStick().GetPosition();
	if (leftStick.GetLengthSquared() > 0.01f)
	{
		Vec3 controllerMove = forward * leftStick.y + left * (-leftStick.x);
		controllerMove = controllerMove.GetNormalized();
		m_position += controllerMove * speed * deltaSeconds;
	}
	
	float verticalInput = controller.GetLeftTrigger() - controller.GetRightTrigger();
	if (fabsf(verticalInput) > 0.01f)
	{
		m_position.z += verticalInput * speed * deltaSeconds;
	}
	
	// Update orientation
	Vec2 mouseDelta = g_theInput->GetCursorClientDelta();
	m_orientation.m_yawDegrees -= mouseDelta.x * 0.075f;
	m_orientation.m_pitchDegrees += mouseDelta.y * 0.075f;
	
	m_orientation.m_yawDegrees -= controller.GetRightStick().GetPosition().x * 90.0f * deltaSeconds;
	m_orientation.m_pitchDegrees += controller.GetRightStick().GetPosition().y * 90.0f * deltaSeconds;
	
	m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -85.0f, 85.0f);
	m_orientation.m_yawDegrees = fmodf(m_orientation.m_yawDegrees, 360.0f);
}

void GameCamera::UpdateIndependent(Entity* entity)
{
	// Player dispossessed - controls move player, camera stays static
	// Camera doesn't move, just keep current position and orientation
	// Player movement is still handled in Player::Update
	
	if (!entity)
		return;
	
	// Camera stays where it is
	// No position/orientation updates
}

Vec3 GameCamera::ResolveThirdPersonCollision(Vec3 const& targetPos, Vec3 const& pivotPos, World* world)
{
	if (!world)
		return targetPos;
	
	// Raycast from pivot to target position
	Vec3 direction = targetPos - pivotPos;
	float maxDistance = direction.GetLength();
	
	if (maxDistance < 0.01f)
		return targetPos;
	
	direction = direction.GetNormalized();
	
	GameRaycastResult3D result = world->RaycastVsBlocks(pivotPos, direction, maxDistance);
	
	if (result.m_didImpact)
	{
		// Hit a block, pull camera closer
		float safeDistance = result.m_impactDist - 0.2f; // Leave some buffer
		safeDistance = GetClamped(safeDistance, m_minOverShoulderDistance, maxDistance);
		return pivotPos + direction * safeDistance;
	}
	
	return targetPos;
}

void GameCamera::AddCameraShake(float intensity, float duration)
{
	m_isCameraShaking = true;
	m_shakeIntensity = intensity;
	m_shakeDuration = duration;
	m_shakeTimer = 0.0f;
}

void GameCamera::UpdateCameraShake(float deltaSeconds)
{
	if (!m_isCameraShaking)
		return;
	
	m_shakeTimer += deltaSeconds;
	
	if (m_shakeTimer >= m_shakeDuration)
	{
		m_isCameraShaking = false;
		m_shakeOffset = Vec3();
		return;
	}
	
	// Calculate decay
	float remainingRatio = 1.0f - (m_shakeTimer / m_shakeDuration);
	float currentIntensity = m_shakeIntensity * remainingRatio;
	
	// Generate random offset
	m_shakeOffset.x = g_theRNG->RollRandomFloatInRange(-currentIntensity, currentIntensity);
	m_shakeOffset.y = g_theRNG->RollRandomFloatInRange(-currentIntensity, currentIntensity);
	m_shakeOffset.z = g_theRNG->RollRandomFloatInRange(-currentIntensity, currentIntensity);
}

void GameCamera::SetCameraMode(GameCameraMode mode)
{
	if (m_cameraMode != mode)
	{
		m_cameraMode = mode;
		
		const char* modeNames[] = {
			"FIRST_PERSON",
			"OVER_SHOULDER",
			"FIXED_ANGLE_TRACKING",
			"SPECTATOR",
			"SPECTATOR_XY",
			"INDEPENDENT"
		};

		m_cameraModeString = Stringf("Camera Mode: %s\n", modeNames[(int)mode]);
		//DebuggerPrintf("Camera Mode: %s\n", modeNames[(int)mode]);
	}
}

void GameCamera::CycleCameraMode()
{
	int currentMode = (int)m_cameraMode;
	currentMode = (currentMode + 1) % 6;
	SetCameraMode((GameCameraMode)currentMode);
}