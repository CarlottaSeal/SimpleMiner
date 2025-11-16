#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vec3.hpp"

class Player;
class World;

enum class CameraViewMode
{
    FIRST_PERSON,
    THIRD_PERSON,
    FREE_FLY  // 用于调试
};

class GameCamera : public Camera
{
public:
    GameCamera();
    ~GameCamera();

    // 核心更新
    void Update(float deltaSeconds);
    
    // 模式切换
    void SetViewMode(CameraViewMode mode);
    CameraViewMode GetViewMode() const { return m_viewMode; }
    void ToggleViewMode();
    
    // 第一人称
    void UpdateFirstPerson(const Vec3& playerPos, const EulerAngles& playerOrientation);
    void SetFirstPersonEyeHeight(float height) { m_firstPersonEyeHeight = height; }
    
    // 第三人称
    void UpdateThirdPerson(const Vec3& playerPos, const EulerAngles& playerOrientation, World* world);
    void SetThirdPersonDistance(float distance) { m_thirdPersonDistance = distance; }
    void SetThirdPersonOffset(const Vec3& offset) { m_thirdPersonPivotOffset = offset; }
    
    // 相机平滑跟随
    void SetSmoothFollowSpeed(float speed) { m_smoothFollowSpeed = speed; }
    void EnableSmoothing(bool enable) { m_enableSmoothing = enable; }
    
    // 相机碰撞（防止穿墙）
    Vec3 ResolveThirdPersonCollision(const Vec3& targetPos, const Vec3& playerPos, World* world);
    
    // 相机抖动（受伤、跳跃等）
    void AddCameraShake(float intensity, float duration);
    void UpdateCameraShake(float deltaSeconds);
    
    // 调试
    void SetDebugFreeFlyCameraEnabled(bool enabled);

private:
    // 当前模式
    CameraViewMode m_viewMode = CameraViewMode::FIRST_PERSON;
    
    // 第一人称参数
    float m_firstPersonEyeHeight = 1.62f;  // Minecraft标准眼高
    
    // 第三人称参数
    float m_thirdPersonDistance = 5.0f;
    Vec3 m_thirdPersonPivotOffset = Vec3(0, 0, 1.5f);  // 相对于玩家脚底的偏移
    float m_minThirdPersonDistance = 1.0f;
    float m_maxThirdPersonDistance = 10.0f;
    
    // 平滑跟随
    bool m_enableSmoothing = true;
    float m_smoothFollowSpeed = 10.0f;
    Vec3 m_smoothedPosition;
    EulerAngles m_smoothedOrientation;
    
    // 相机抖动
    bool m_isCameraShaking = false;
    float m_shakeIntensity = 0.0f;
    float m_shakeDuration = 0.0f;
    float m_shakeTimer = 0.0f;
    Vec3 m_shakeOffset;
    
    // 调试自由飞行
    bool m_debugFreeFlyCameraEnabled = false;
};