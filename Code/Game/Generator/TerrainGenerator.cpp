// TerrainGenerator.cpp - 添加了Z Bias的完整版本

#include "TerrainGenerator.h"

#include "Engine/Math/Curve1D.h"
#include "ThirdParty/Noise/SmoothNoise.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

extern Game* g_theGame;

TerrainGenerator::TerrainGenerator(unsigned int baseSeed)
    : m_biomeGenerator(baseSeed)
{
    m_densitySeed = baseSeed + 100;
    
    m_continentHeightOffsetCurve = CreateContinentHeightOffsetCurve();
    m_continentSquashingCurve = CreateContinentSquashingCurve();
}

TerrainGenerator::~TerrainGenerator()
{
    delete m_continentHeightOffsetCurve;
    delete m_continentSquashingCurve;
}

Curve1D* TerrainGenerator::CreateContinentHeightOffsetCurve()
{
    std::vector<Vec2> points = g_theGame->g_heightOffsetCurvePoints;
    if (points.empty())
        return nullptr;
    
    auto* curve = new PiecewiseCurve1D();
    
    for (size_t i = 0; i < points.size() - 1; i++)
    {
        float t = points[i].x;        
        float startY = points[i].y;   
        float endY = points[i + 1].y; 
        
        curve->AddKeyPoint(t, new LinearCurve1D(startY, endY));
    }
    
    float lastT = points.back().x;
    float lastY = points.back().y;
    curve->AddKeyPoint(lastT, new LinearCurve1D(lastY, lastY));
    
    return curve;
}

Curve1D* TerrainGenerator::CreateContinentSquashingCurve()
{
    std::vector<Vec2> points = g_theGame->g_heightScaleCurvePoints;
    if (points.empty())
        return nullptr;
    
    auto* curve = new PiecewiseCurve1D();
    
    for (size_t i = 0; i < points.size() - 1; i++)
    {
        float t = points[i].x;        
        float startY = points[i].y;   
        float endY = points[i + 1].y; 
        
        curve->AddKeyPoint(t, new LinearCurve1D(startY, endY));
    }
    
    float lastT = points.back().x;
    float lastY = points.back().y;
    curve->AddKeyPoint(lastT, new LinearCurve1D(lastY, lastY));
    
    return curve;
}

float TerrainGenerator::GetZBias(float z)
{
    // Blog公式：(z - CHUNK_SIZE_Z) * (2.0 / CHUNK_SIZE_Z)
    // 
    // 这个公式的作用：
    // - z = 0   (底部) → bias = -2.0 → 密度减小 → 更容易是固体
    // - z = 64  (中部) → bias = -1.0 → 中性
    // - z = 128 (顶部) → bias = 0.0  → 密度增大 → 更容易是空气 

    // float center = (float)CHUNK_SIZE_Z / 2.0f;  // 64
    // return (z - center) * (2.0f / (float)CHUNK_SIZE_Z);
    return (z - g_theGame->g_terrainHeight) * g_theGame->g_biasPerZ;
}

float TerrainGenerator::Calculate3DDensity(
    const Vec3& worldPos, 
    const BiomeGenerator::BiomeParameters& biomeParams)
{
    float noiseValue = 0.f;
    if (g_theGame->g_densityNoiseEnabled)
    {
        noiseValue = Compute3dPerlinNoise(
        worldPos.x, worldPos.y, worldPos.z,
        g_theGame->g_densityNoiseScale,  // Scale
        g_theGame->g_densityNoiseOctaves,       // Octaves
        0.5f,    // Persistence
        2.0f,    // Lacunarity
        true,    // Renormalize
        m_densitySeed
        );
    }

    float density = noiseValue;
    if (g_theGame->g_densityNoiseBiasEnabled)
    {
        float zBias = GetZBias(worldPos.z);
        density += zBias;
    }
    
    float h = m_continentHeightOffsetCurve->Evaluate(biomeParams.m_continentalness);
    float s = m_continentSquashingCurve->Evaluate(biomeParams.m_continentalness);
    
    float default_terrain_height = g_theGame->g_terrainReferenceHeight;
    float b = default_terrain_height + (h * g_theGame->g_terrainHeight);
    float t = (worldPos.z - b) / b;

    if (g_theGame->g_continentHeightOffsetEnabled)
    {
        density -= h;        // Height offset
    }
    if (g_theGame->g_continentHeightScaleEnabled)
    {
        density += s * t;    // Squashing

        if (worldPos.z < g_theGame->g_seaLevel - 10)
        {
            float depthBelowSea = (g_theGame->g_seaLevel - 10) - worldPos.z;
            float depthFactor = depthBelowSea * 0.02f;
            density -= depthFactor;
        }
    
        // PVType pvType = ClassifyPV(biomeParams.m_peaksAndValleys);
        // ErosionType erosionType = ClassifyErosion(biomeParams.m_erosion);
        //
        // if (pvType == PV_PEAKS && erosionType <= E1)
        // {
        //     float estimatedSurfaceHeight = b;
        //     float heightAboveSurface = worldPos.z - estimatedSurfaceHeight;
        //
        //     // 只在地表附近折叠
        //     if (heightAboveSurface > -10.0f && heightAboveSurface < 25.0f)
        //     {
        //         float strength = 1.0f - (fabsf(heightAboveSurface) / 25.0f);
        //         density = FoldDensity(density, 0.25f, strength);
        //     }
        // }
    }
    
    return density;
}

float TerrainGenerator::FoldDensity(float density, float threshold, float strength)
{
    if (density > threshold)
    {
        float excess = density - threshold;

        float t = GetClamped(excess / 0.5f, 0.0f, 1.0f); 
        //float smoothT = SmoothStop3(t);
        float smoothT = t * t * (3.0f - 2.0f * t); 
        return threshold - (excess * smoothT * strength);
    }
    return density;
}

int TerrainGenerator::GetSurfaceHeight(
    int worldX, 
    int worldY, 
    const BiomeGenerator::BiomeParameters& biomeParams)
{
    // 这是一个近似计算，用于预判地表位置
    // 实际地表由3D密度决定
    UNUSED(worldX);
    UNUSED(worldY);
    
    float h = m_continentHeightOffsetCurve->Evaluate(biomeParams.m_continentalness);
    
    float default_terrain_height = (float)CHUNK_SIZE_Z;
    float estimatedHeight = default_terrain_height + (h * ((float)CHUNK_SIZE_Z / 2.0f));
    
    // 限制在合理范围
    if (estimatedHeight < 0) estimatedHeight = 0;
    if (estimatedHeight >= CHUNK_SIZE_Z) estimatedHeight = CHUNK_SIZE_Z - 1;
    
    return (int)estimatedHeight;
}

PVType TerrainGenerator::ClassifyPV(float pv)
{
    if (pv < -0.85f)
        return PV_VALLEYS;
    else if (pv < -0.2f)
        return PV_LOW;
    else if (pv < 0.2f)
        return PV_MID;
    else if (pv < 0.7f)
        return PV_HIGH;
    else
        return PV_PEAKS;
}

ErosionType TerrainGenerator::ClassifyErosion(float e)
{
    if (e < -0.78f)
        return E0;
    else if (e < -0.375f)
        return E1;
    else if (e < -0.2225f)
        return E2;
    else if (e < 0.05f)
        return E3;
    else if (e < 0.45f)
        return E4;
    else if (e < 0.55f)
        return E5;
    else
        return E6;
}