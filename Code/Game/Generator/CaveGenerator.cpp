#include "CaveGenerator.h"

#include "Engine/Math/Vec3.hpp"
#include "Game/Block.h"
#include "Game/Gamecommon.hpp"
#include "Game/ChunkUtils.h"
#include "ThirdParty/Noise/SmoothNoise.hpp"
#include "Game/Game.hpp"
#include <cmath>

extern Game* g_theGame;

CaveGenerator::CaveGenerator(unsigned int baseSeed)
{
    m_cheeseSeed = baseSeed + 300;
    m_spaghettiSeed = baseSeed + 301;
    m_noodleSeed = baseSeed + 302;
    m_aquiferSeed = baseSeed + 303;
}

int CaveGenerator::EstimateDistanceToSurface(
    Block* blocks,
    int localX, int localY, int localZ)
{
    int minDist = 999;
    
    // 1. 向上搜索
    for (int dz = 0; dz < 30 && localZ + dz < CHUNK_SIZE_Z; dz++)
    {
        int checkIdx = LocalCoordsToIndex(localX, localY, localZ + dz);
        if (!IsSolid(blocks[checkIdx].m_typeIndex))
        {
            minDist = (dz < minDist) ? dz : minDist;
            break;
        }
    }
    
    // 2. 向下搜索
    for (int dz = 0; dz < 20 && localZ - dz >= 0; dz++)
    {
        int checkIdx = LocalCoordsToIndex(localX, localY, localZ - dz);
        if (!IsSolid(blocks[checkIdx].m_typeIndex))
        {
            minDist = (dz < minDist) ? dz : minDist;
            break;
        }
    }
    
    // 3. 水平方向（简化版，只检查主要方向）
    const int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
    
    for (int dir = 0; dir < 4; dir++)
    {
        int dx = directions[dir][0];
        int dy = directions[dir][1];
        
        for (int dist = 1; dist <= 15; dist++)
        {
            int checkX = localX + dx * dist;
            int checkY = localY + dy * dist;
            
            if (checkX < 0 || checkX >= CHUNK_SIZE_X ||
                checkY < 0 || checkY >= CHUNK_SIZE_Y)
            {
                break;
            }
            
            int checkIdx = LocalCoordsToIndex(checkX, checkY, localZ);
            if (!IsSolid(blocks[checkIdx].m_typeIndex))
            {
                minDist = (dist < minDist) ? dist : minDist;
                break;
            }
        }
    }
    
    return minDist;
}

// ===== 改进1：更平滑的阈值函数 =====
float CaveGenerator::SmoothThreshold(float value, float threshold, float smoothness)
{
    float diff = value - threshold;
    if (diff <= -smoothness) return 0.0f;
    if (diff >= smoothness) return 1.0f;
    
    // 使用平滑插值
    float t = (diff + smoothness) / (2.0f * smoothness);
    return t * t * (3.0f - 2.0f * t); // Smoothstep
}

// ===== 改进2：基于高度的深度因子 =====
float CaveGenerator::GetDepthFactor(float worldZ)
{
    float seaLevel = (float)g_theGame->g_seaLevel;
    
    // 地表附近：很少洞穴
    if (worldZ > seaLevel + 10.0f)
    {
        return 0.0f;
    }
    else if (worldZ > seaLevel - 10.0f)
    {
        // 海平面附近：线性增长
        float t = (seaLevel + 10.0f - worldZ) / 20.0f;
        return t * 0.3f;
    }
    else if (worldZ > seaLevel - 40.0f)
    {
        // 中等深度：适中密度
        float t = (seaLevel - 10.0f - worldZ) / 30.0f;
        return 0.3f + t * 0.4f;
    }
    else if (worldZ > 10.0f)
    {
        // 深层：高密度
        float t = (seaLevel - 40.0f - worldZ) / (seaLevel - 50.0f);
        t = (t > 1.0f) ? 1.0f : t;
        return 0.7f + t * 0.25f;
    }
    else
    {
        // 接近基岩：减少洞穴
        return 0.95f * (worldZ / 10.0f);
    }
}

// ===== 改进3：密度调节器（增加自然变化）=====
float CaveGenerator::GetDensityModifier(const Vec3& worldPos)
{
    // 使用大尺度噪声创造区域变化
    float regionalNoise = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        200.0f, // 大尺度
        3, 0.5f, 2.0f, true, m_cheeseSeed + 500
    );
    
    // 将 [-1, 1] 映射到 [0.6, 1.4]
    return 1.0f + regionalNoise * 0.4f;
}

// ===== 改进4：Cheese洞穴 - 多频率叠加 =====
float CaveGenerator::GetCheeseNoise(const Vec3& worldPos)
{
    // 主要噪声（大型结构）
    float mainNoise = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        50.0f, 4, 0.5f, 2.0f, true, m_cheeseSeed
    );
    
    // 次要噪声（细节变化）
    float detailNoise = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        25.0f, 3, 0.5f, 2.0f, true, m_cheeseSeed + 100
    );
    
    // 组合噪声
    float combined = mainNoise * 0.7f + detailNoise * 0.3f;
    
    // 归一化到 [0, 1]
    return (combined + 1.0f) * 0.5f;
}

// ===== 改进5：Spaghetti洞穴 - 改进的隧道算法 =====
float CaveGenerator::GetSpaghettiNoise(const Vec3& worldPos)
{
    // 两个独立的噪声场用于创建蜿蜒隧道
    float noise1 = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        18.0f, 3, 0.6f, 2.0f, true, m_spaghettiSeed
    );
    
    float noise2 = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        18.0f, 3, 0.6f, 2.0f, true, m_spaghettiSeed + 1000
    );
    
    // 计算到隧道中心的距离
    float distance = sqrtf(noise1 * noise1 + noise2 * noise2);
    
    // 隧道半径（可变）
    float radiusNoise = Compute3dPerlinNoise(
        worldPos.x * 0.1f, worldPos.y * 0.1f, worldPos.z * 0.1f,
        10.0f, 2, 0.5f, 2.0f, true, m_spaghettiSeed + 2000
    );
    float radius = 0.08f + radiusNoise * 0.04f; // 半径在0.04-0.12之间变化
    
    // 返回归一化的"隧道性"值
    if (distance < radius)
    {
        return 1.0f - (distance / radius);
    }
    return 0.0f;
}

// ===== 改进6：Noodle洞穴 - 更细的隧道 =====
float CaveGenerator::GetNoodleNoise(const Vec3& worldPos)
{
    float noise1 = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        10.0f, 2, 0.6f, 2.0f, true, m_noodleSeed
    );
    
    float noise2 = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        10.0f, 2, 0.6f, 2.0f, true, m_noodleSeed + 1000
    );
    
    float distance = sqrtf(noise1 * noise1 + noise2 * noise2);
    
    // 更细的隧道
    float radius = 0.05f;
    
    if (distance < radius)
    {
        return 1.0f - (distance / radius);
    }
    return 0.0f;
}

// ===== 新增：含水层判断 =====
bool CaveGenerator::ShouldCarveAquifer(const Vec3& worldPos, float seaLevel)
{
    if (worldPos.z > seaLevel - 5.0f)
    {
        return false; // 接近海平面，不是含水层
    }
    
    // 使用噪声判断是否是含水层
    float aquiferNoise = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        80.0f, 3, 0.5f, 2.0f, true, m_aquiferSeed
    );
    
    // 含水层出现在某些区域
    return aquiferNoise > 0.2f;
}

// ===== 核心判断函数：是否在洞穴中 =====
bool CaveGenerator::IsInCave(const Vec3& worldPos, int distanceToSurface, float* outCaveness)
{
    // 安全检查
    if (worldPos.z < 5 || worldPos.z > 120)
    {
        if (outCaveness) *outCaveness = 0.0f;
        return false;
    }
    
    // 必须离表面有一定距离
    if (distanceToSurface < 5)
    {
        if (outCaveness) *outCaveness = 0.0f;
        return false;
    }
    
    // 获取深度因子
    float depthFactor = GetDepthFactor(worldPos.z);
    if (depthFactor < 0.01f)
    {
        if (outCaveness) *outCaveness = 0.0f;
        return false;
    }
    
    // 获取密度调节器
    float densityMod = GetDensityModifier(worldPos);
    
    // 计算各类洞穴的贡献
    float cheeseCaveness = 0.0f;
    float spaghettiCaveness = 0.0f;
    float noodleCaveness = 0.0f;
    
    float seaLevel = (float)g_theGame->g_seaLevel;
    float depthBelowSea = seaLevel - worldPos.z;
    
    // === Cheese洞穴（大型空洞）===
    if (depthBelowSea > 15.0f)
    {
        float cheeseValue = GetCheeseNoise(worldPos);
        // 自适应阈值
        float cheeseThreshold = 0.55f - depthFactor * 0.15f;
        cheeseCaveness = SmoothThreshold(cheeseValue, cheeseThreshold, 0.05f);
    }
    
    // === Spaghetti洞穴（主要隧道）===
    if (depthBelowSea > 8.0f)
    {
        spaghettiCaveness = GetSpaghettiNoise(worldPos);
        // 深度越深，越容易出现
        spaghettiCaveness *= depthFactor * densityMod;
    }
    
    // === Noodle洞穴（细隧道）===
    if (distanceToSurface >= 12 && depthBelowSea > 20.0f)
    {
        noodleCaveness = GetNoodleNoise(worldPos);
        noodleCaveness *= depthFactor;
    }
    
    // 组合所有洞穴类型
    float totalCaveness = cheeseCaveness * 0.8f + 
                          spaghettiCaveness * 0.9f + 
                          noodleCaveness * 0.7f;
    
    // 应用距离衰减（离表面近的地方减少洞穴）
    float distanceFactor = 1.0f;
    if (distanceToSurface < 10)
    {
        distanceFactor = (float)distanceToSurface / 10.0f;
    }
    totalCaveness *= distanceFactor;
    
    if (outCaveness)
    {
        *outCaveness = totalCaveness;
    }
    
    // 阈值判断
    return totalCaveness > 0.4f;
}

// ===== 新增：石柱噪声（用于大型洞穴） =====
float CaveGenerator::GetPillarNoise(const Vec3& worldPos)
{
    // 垂直方向的噪声，创造类似钟乳石/石笋的效果
    float pillarNoise = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z * 0.3f, // Z轴拉伸
        15.0f, 3, 0.5f, 2.0f, true, m_cheeseSeed + 999
    );
    
    return pillarNoise;
}

// ===== 主函数：雕刻洞穴 =====
void CaveGenerator::CarveCaves(Block* blocks, const IntVec2& chunkCoords)
{
    int caveCount = 0;
    int waterCaveCount = 0;
    int airCaveCount = 0;
    int lavaCaveCount = 0;
    
    // 统计洞穴类型
    int cheeseCount = 0;
    int spaghettiCount = 0;
    int noodleCount = 0;
    
    float seaLevel = (float)g_theGame->g_seaLevel;
    
    for (int z = 0; z < CHUNK_SIZE_Z; z++)
    {
        for (int y = 0; y < CHUNK_SIZE_Y; y++)
        {
            for (int x = 0; x < CHUNK_SIZE_X; x++)
            {
                int idx = LocalCoordsToIndex(x, y, z);
                uint8_t blockType = blocks[idx].m_typeIndex;
                
                // 只处理固体方块
                if (!IsSolid(blockType))
                {
                    continue;
                }
                
                // 计算世界坐标
                int worldX = chunkCoords.x * CHUNK_SIZE_X + x;
                int worldY = chunkCoords.y * CHUNK_SIZE_Y + y;
                Vec3 worldPos((float)worldX, (float)worldY, (float)z);
                
                // 估算到表面的距离
                int distanceToSurface = EstimateDistanceToSurface(blocks, x, y, z);
                
                // 判断是否在洞穴中
                float caveness = 0.0f;
                if (IsInCave(worldPos, distanceToSurface, &caveness))
                {
                    caveCount++;
                    
                    // 统计类型（简化版）
                    float depthBelowSea = seaLevel - z;
                    if (depthBelowSea > 15.0f && GetCheeseNoise(worldPos) > 0.6f)
                        cheeseCount++;
                    if (depthBelowSea > 8.0f && GetSpaghettiNoise(worldPos) > 0.3f)
                        spaghettiCount++;
                    if (GetNoodleNoise(worldPos) > 0.3f)
                        noodleCount++;
                    
                    // 决定填充类型
                    if (z < 10)
                    {
                        // 深层：岩浆
                        blocks[idx].m_typeIndex = BLOCK_TYPE_LAVA;
                        lavaCaveCount++;
                    }
                    else if (ShouldCarveAquifer(worldPos, seaLevel) && z < seaLevel - 10)
                    {
                        // 含水层：水
                        blocks[idx].m_typeIndex = BLOCK_TYPE_WATER;
                        waterCaveCount++;
                    }
                    else
                    {
                        // 默认：空气
                        blocks[idx].m_typeIndex = BLOCK_TYPE_AIR;
                        airCaveCount++;
                    }
                    
                    // 可选：在大型洞穴中保留一些石柱
                    if (caveness > 0.7f && cheeseCount > 0)
                    {
                        float pillarValue = GetPillarNoise(worldPos);
                        if (pillarValue > 0.75f)
                        {
                            // 保持为石头（石柱效果）
                            blocks[idx].m_typeIndex = blockType;
                            caveCount--;
                        }
                    }
                }
            }
        }
    }
}