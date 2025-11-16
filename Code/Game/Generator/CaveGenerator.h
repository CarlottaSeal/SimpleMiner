#pragma once

struct IntVec2;
class Block;
struct Vec3;

class CaveGenerator
{
public:
    enum CaveType
    {
        CAVE_CHEESE,     // 奶酪洞穴（大型空洞）
        CAVE_SPAGHETTI,  // 意面洞穴（蜿蜒通道）
        CAVE_NOODLE      // 面条洞穴（更细的通道）
    };

    CaveGenerator(unsigned int baseSeed);
    
    // 核心洞穴生成
    void CarveCaves(Block* blocks, const IntVec2& chunkCoords);
    bool IsInCave(const Vec3& worldPos, int distanceToSurface, float* outCaveness = nullptr);
    
    // 表面检测
    int EstimateDistanceToSurface(Block* blocks, int localX, int localY, int localZ);
    
private:
    // ===== 噪声生成函数 =====
    float GetCheeseNoise(const Vec3& worldPos);
    float GetSpaghettiNoise(const Vec3& worldPos);
    float GetNoodleNoise(const Vec3& worldPos);
    
    // ===== 辅助函数 =====
    float GetDepthFactor(float worldZ);
    float GetDensityModifier(const Vec3& worldPos);
    float SmoothThreshold(float value, float threshold, float smoothness = 0.1f);
    
    // ===== 新增：高级洞穴特性 =====
    bool ShouldCarveAquifer(const Vec3& worldPos, float seaLevel);
    float GetPillarNoise(const Vec3& worldPos); // 用于在大型洞穴中生成石柱
    
private:
    unsigned int m_cheeseSeed;
    unsigned int m_spaghettiSeed;
    unsigned int m_noodleSeed;
    unsigned int m_aquiferSeed;
};