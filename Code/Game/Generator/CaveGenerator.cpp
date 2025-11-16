#include "CaveGenerator.h"
#include "Game/Game.hpp"
#include "WorldGenPipeline.h"
#include "Game/Block.h"
#include "Game/ChunkUtils.h"
#include "ThirdParty/Noise/SmoothNoise.hpp"

extern Game* g_theGame;

CaveGenerator::CaveGenerator(unsigned int seed)
    : m_cheeseSeed(seed)
    , m_spaghettiSeed(seed + 1000)
    , m_noodleSeed(seed + 2000)
    , m_densitySeed(seed + 3000)
{
}

bool CaveGenerator::IsInCave(const Vec3& worldPos, int distanceToSurface, float terrainHeight, float* outCaveness)
{
    float seaLevel = (float)g_theGame->g_seaLevel;
    
    if (worldPos.z < 5 || worldPos.z > 200)
    {
        return false;
    }
    
    if (distanceToSurface < 2)
    {
        return false;
    }
    
    if (terrainHeight < seaLevel)
    {
        if (worldPos.z >= terrainHeight - 5.0f)
        {
            return false;  // 海底以上不生成洞穴
        }
    }
    
    float depthBelowSea = seaLevel - worldPos.z;
    if (depthBelowSea < -5.0f)
    {
        return false;  
    }
    
    float caveDensityNoise = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, 0.0f,  // Z=0，只在XY平面变化
        180.0f,  // 超大尺度，形成"洞穴群系"
        3, 0.5f, 2.0f, true, m_densitySeed
    );
    
    // 计算洞穴区域因子
    float caveRegionFactor = 0.0f;
    if (caveDensityNoise > 0.5f)
    {
        caveRegionFactor = 1.0f;  // 密集洞穴区
    }
    else if (caveDensityNoise > 0.3f)
    {
        caveRegionFactor = 0.6f;  // 正常洞穴区
    }
    else if (caveDensityNoise > 0.1f)
    {
        caveRegionFactor = 0.3f;  // 稀疏洞穴区
    }
    else
    {
        caveRegionFactor = 0.0f;  // 无洞穴区
    }
    
    if (terrainHeight < seaLevel)
    {
        caveRegionFactor *= 0.5f;
    }
    
    if (caveRegionFactor < 0.01f)
    {
        return false;  
    }
    
    // ===== 第2步：垂直分层权重（深度控制）=====
    float depth = worldPos.z;
    
    float cheeseDepthWeight = 0.0f;
    float spaghettiDepthWeight = 0.0f;
    float noodleDepthWeight = 0.0f;
    
    if (depth < 20.0f)
    {
        // 深层（0-20）：80% Cheese，20% Spaghetti
        cheeseDepthWeight = 0.8f;
        spaghettiDepthWeight = 0.2f;
        noodleDepthWeight = 0.0f;
    }
    else if (depth < 40.0f)
    {
        // 探索层（20-40）：渐变到 30% Cheese，60% Spaghetti，10% Noodle
        float t = (depth - 20.0f) / 20.0f;  // 0→1
        cheeseDepthWeight = Interpolate(0.8f, 0.3f, t);
        spaghettiDepthWeight = Interpolate(0.2f, 0.6f, t);
        noodleDepthWeight = Interpolate(0.0f, 0.1f, t);
    }
    else if (depth < 60.0f)
    {
        // 浅层（40-60）：渐变到 0% Cheese，70% Spaghetti，30% Noodle
        float t = (depth - 40.0f) / 20.0f;  // 0→1
        cheeseDepthWeight = Interpolate(0.3f, 0.0f, t);
        spaghettiDepthWeight = Interpolate(0.6f, 0.7f, t);
        noodleDepthWeight = Interpolate(0.1f, 0.3f, t);
    }
    else
    {
        // 地表（60+）：几乎没有洞穴，只有入口
        cheeseDepthWeight = 0.0f;
        spaghettiDepthWeight = 0.1f;
        noodleDepthWeight = 0.0f;
    }
    
    // ===== 第3步：生成各类型洞穴 =====
    
    // ----- Cheese大空洞 -----
    float cheeseNoise1 = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        50.0f,  // 中等尺度
        4, 0.5f, 2.0f, true, m_cheeseSeed
    );
    
    float cheeseNoise2 = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        70.0f,  // 大尺度
        3, 0.5f, 2.0f, true, m_cheeseSeed + 100
    );
    
    float cheeseness = (cheeseNoise1 * 0.6f + cheeseNoise2 * 0.4f);
    bool isCheese = (cheeseness > 0.48f);
    
    // ----- Spaghetti隧道（修复的算法）-----
    float spaghettiX = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        28.0f,
        5, 0.6f, 2.0f, true, m_spaghettiSeed
    );
    
    float spaghettiY = Compute3dPerlinNoise(
        worldPos.x + 100.0f, worldPos.y + 100.0f, worldPos.z,
        28.0f,
        5, 0.6f, 2.0f, true, m_spaghettiSeed + 50
    );
    
    float spaghettiZ = Compute3dPerlinNoise(
        worldPos.x + 200.0f, worldPos.y + 200.0f, worldPos.z + 100.0f,
        20.0f,
        4, 0.5f, 2.0f, true, m_spaghettiSeed + 100
    );
    
    // 关键修复：使用绝对值，当两个都接近0时生成隧道
    float spaghettiness = fabsf(spaghettiX) * fabsf(spaghettiY);
    bool isSpaghetti = (spaghettiness < 0.08f && fabsf(spaghettiZ) < 0.6f);
    
    // ----- Noodle细通道 -----
    float noodleNoise = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        18.0f,
        6, 0.65f, 2.0f, true, m_noodleSeed
    );
    
    float noodleness = fabsf(noodleNoise);
    bool isNoodle = (noodleness < 0.12f);
    
    // ===== 第4步：组合判断（应用分层权重）=====
    float totalCaveness = 0.0f;
    
    if (isCheese && cheeseDepthWeight > 0.001f)
    {
        float weight = cheeseDepthWeight * caveRegionFactor;
        float contribution = (cheeseness - 0.48f) / 0.52f;
        totalCaveness += weight * contribution;
    }
    
    if (isSpaghetti && spaghettiDepthWeight > 0.001f)
    {
        float weight = spaghettiDepthWeight * caveRegionFactor;
        float contribution = (0.08f - spaghettiness) / 0.08f;
        totalCaveness += weight * contribution;
    }
    
    if (isNoodle && noodleDepthWeight > 0.001f)
    {
        float weight = noodleDepthWeight * caveRegionFactor;
        float contribution = (0.12f - noodleness) / 0.12f;
        totalCaveness += weight * contribution;
    }
    
    if (outCaveness)
    {
        *outCaveness = totalCaveness;
    }
    
    // 最终阈值判断
    return totalCaveness > 0.3f;
}

void CaveGenerator::CarveCaves(Block* blocks, const IntVec2& chunkCoords, const ChunkGenData& chunkGenData)
{
    int caveCount = 0;
    int airCaveCount = 0;
    int waterCaveCount = 0;
    int lavaCaveCount = 0;
    
    int cheeseCount = 0;
    int spaghettiCount = 0;
    int noodleCount = 0;
    
    float seaLevel = (float)g_theGame->g_seaLevel;
    
    for (int z = 2; z < CHUNK_SIZE_Z - 1; z++)
    {
        for (int y = 0; y < CHUNK_SIZE_Y; y++)
        {
            for (int x = 0; x < CHUNK_SIZE_X; x++)
            {
                int idx = LocalCoordsToIndex(x, y, z);
                
                if (blocks[idx].m_typeIndex != BLOCK_TYPE_STONE &&
                    blocks[idx].m_typeIndex != BLOCK_TYPE_DIRT &&
                    blocks[idx].m_typeIndex != BLOCK_TYPE_SAND)
                {
                    continue;
                }
                
                // 计算世界坐标
                float worldX = (float)(chunkCoords.x * CHUNK_SIZE_X + x);
                float worldY = (float)(chunkCoords.y * CHUNK_SIZE_Y + y);
                float worldZ = (float)z;
                Vec3 worldPos(worldX, worldY, worldZ);
                
                // ===== 从ChunkGenData获取地形高度 =====
                float terrainHeight = (float)chunkGenData.m_surfaceHeights[x][y];
                
                // 计算到地表的距离
                int distanceToSurface = CalculateDistanceToSurface(blocks, x, y, z);
                
                // 检查是否在洞穴中
                float caveness = 0.0f;
                if (!IsInCave(worldPos, distanceToSurface, terrainHeight, &caveness))
                {
                    continue;
                }
                
                caveCount++;
                
                // ===== 液体填充规则 =====
                float depthBelowSea = seaLevel - worldZ;
                
                if (worldZ < 10)
                {
                    // 深层：岩浆
                    blocks[idx].m_typeIndex = BLOCK_TYPE_LAVA;
                    lavaCaveCount++;
                }
                else if (worldZ < 20 && caveness > 0.7f)
                {
                    // 中深层大洞穴：少量水
                    float waterNoise = Compute3dPerlinNoise(
                        worldX, worldY, worldZ,
                        40.0f, 2, 0.5f, 2.0f, true, 99999
                    );
                    
                    if (waterNoise > 0.65f)
                    {
                        blocks[idx].m_typeIndex = BLOCK_TYPE_WATER;
                        waterCaveCount++;
                    }
                    else
                    {
                        blocks[idx].m_typeIndex = BLOCK_TYPE_AIR;
                        airCaveCount++;
                    }
                }
                else
                {
                    // 正常洞穴：空气
                    blocks[idx].m_typeIndex = BLOCK_TYPE_AIR;
                    airCaveCount++;
                }
                
                // 统计类型
                if (caveness > 0.5f)
                    cheeseCount++;
                else if (caveness > 0.3f)
                    spaghettiCount++;
                else
                    noodleCount++;
            }
        }
    }
    
    if (caveCount > 0)
    {
        DebuggerPrintf("Chunk(%d,%d): %d caves | Air:%d Water:%d Lava:%d | C:%d S:%d N:%d\n",
                       chunkCoords.x, chunkCoords.y,
                       caveCount, airCaveCount, waterCaveCount, lavaCaveCount,
                       cheeseCount, spaghettiCount, noodleCount);
    }
}

int CaveGenerator::CalculateDistanceToSurface(Block* blocks, int x, int y, int z)
{
    int distance = 0;
    
    for (int checkZ = z + 1; checkZ < CHUNK_SIZE_Z; checkZ++)
    {
        int idx = LocalCoordsToIndex(x, y, checkZ);
        uint8_t blockType = blocks[idx].m_typeIndex;
        
        if (blockType == BLOCK_TYPE_AIR || 
            blockType == BLOCK_TYPE_WATER)
        {
            return distance;
        }
        
        distance++;
    }
    
    return distance;
}

float CaveGenerator::Interpolate(float a, float b, float t)
{
    return a + (b - a) * t;
}