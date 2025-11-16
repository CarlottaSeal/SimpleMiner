#include "GameCamera.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/World.h"

extern RandomNumberGenerator* g_theRNG;

GameCamera::GameCamera()
{
    m_smoothedPosition = m_position;
    m_smoothedOrientation = m_orientation;
}

GameCamera::~GameCamera()
{
}

void GameCamera::Update(float deltaSeconds)
{
    // 更新相机抖动
    if (m_isCameraShaking)
    {
        UpdateCameraShake(deltaSeconds);
    }
    
    // 更新Frustum
    UpdateFrustum();
}

void GameCamera::SetViewMode(CameraViewMode mode)
{
    if (m_viewMode != mode)
    {
        m_viewMode = mode;
        
        // 切换模式时重置平滑位置
        m_smoothedPosition = m_position;
        m_smoothedOrientation = m_orientation;
    }
}

void GameCamera::ToggleViewMode()
{
    switch (m_viewMode)
    {
        case CameraViewMode::FIRST_PERSON:
            SetViewMode(CameraViewMode::THIRD_PERSON);
            break;
        case CameraViewMode::THIRD_PERSON:
            SetViewMode(CameraViewMode::FIRST_PERSON);
            break;
        default:
            SetViewMode(CameraViewMode::FIRST_PERSON);
            break;
    }
}

void GameCamera::UpdateFirstPerson(const Vec3& playerPos, const EulerAngles& playerOrientation)
{
    // 第一人称：相机位于玩家眼睛位置
    Vec3 eyePosition = playerPos + Vec3(0, 0, m_firstPersonEyeHeight);
    
    // 添加相机抖动
    if (m_isCameraShaking)
    {
        eyePosition += m_shakeOffset;
    }
    
    if (m_enableSmoothing)
    {
        // 平滑插值
        m_smoothedPosition = InterpolateVec3(m_smoothedPosition, eyePosition, m_smoothFollowSpeed * 0.016f);
        m_smoothedOrientation.m_yawDegrees = GetTurnedTowardDegrees(
            m_smoothedOrientation.m_yawDegrees, 
            playerOrientation.m_yawDegrees, 
            m_smoothFollowSpeed * 0.016f
        );
        m_smoothedOrientation.m_pitchDegrees = GetTurnedTowardDegrees(
            m_smoothedOrientation.m_pitchDegrees, 
            playerOrientation.m_pitchDegrees, 
            m_smoothFollowSpeed * 0.016f
        );
        
        SetPosition(m_smoothedPosition);
        SetOrientation(m_smoothedOrientation);
    }
    else
    {
        SetPosition(eyePosition);
        SetOrientation(playerOrientation);
    }
}

void GameCamera::UpdateThirdPerson(const Vec3& playerPos, const EulerAngles& playerOrientation, World* world)
{
    // 第三人称：相机在玩家后方
    Vec3 pivotPoint = playerPos + m_thirdPersonPivotOffset;
    
    // 计算相机目标位置（玩家后方）
    Vec3 forward, left, up;
    playerOrientation.GetAsVectors_IFwd_JLeft_KUp(forward, left, up);
    
    Vec3 targetPosition = pivotPoint - forward * m_thirdPersonDistance;
    targetPosition += up * 0.5f;  // 稍微向上偏移
    
    // 碰撞检测（防止相机穿墙）
    if (world)
    {
        targetPosition = ResolveThirdPersonCollision(targetPosition, pivotPoint, world);
    }
    
    // 添加相机抖动
    if (m_isCameraShaking)
    {
        targetPosition += m_shakeOffset;
    }
    
    if (m_enableSmoothing)
    {
        m_smoothedPosition = InterpolateVec3(m_smoothedPosition, targetPosition, m_smoothFollowSpeed * 0.016f);
        SetPosition(m_smoothedPosition);
    }
    else
    {
        SetPosition(targetPosition);
    }
    
    SetOrientation(playerOrientation);
}

Vec3 GameCamera::ResolveThirdPersonCollision(const Vec3& targetPos, const Vec3& playerPos, World* world)
{
    // 从玩家位置到相机目标位置做射线检测
    Vec3 direction = targetPos - playerPos;
    float maxDistance = direction.GetLength();
    direction.GetNormalized();
    
    GameRaycastResult3D result = world->RaycastVsBlocks(playerPos, direction, maxDistance);
    
    if (result.m_didImpact)
    {
        // 碰到方块，将相机拉近
        float safeDistance = result.m_impactDist - 0.2f;  // 留一点缓冲
        safeDistance = GetClamped(safeDistance, m_minThirdPersonDistance, maxDistance);
        return playerPos + direction * safeDistance;
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
    
    // 计算衰减
    float remainingRatio = 1.0f - (m_shakeTimer / m_shakeDuration);
    float currentIntensity = m_shakeIntensity * remainingRatio;
    
    // 生成随机偏移
    m_shakeOffset.x = g_theRNG->RollRandomFloatInRange(-currentIntensity, currentIntensity);
    m_shakeOffset.y = g_theRNG->RollRandomFloatInRange(-currentIntensity, currentIntensity);
    m_shakeOffset.z = g_theRNG->RollRandomFloatInRange(-currentIntensity, currentIntensity);
}

void GameCamera::SetDebugFreeFlyCameraEnabled(bool enabled)
{
    m_debugFreeFlyCameraEnabled = enabled;
    if (enabled)
    {
        SetViewMode(CameraViewMode::FREE_FLY);
    }
    else
    {
        SetViewMode(CameraViewMode::FIRST_PERSON);
    }
}