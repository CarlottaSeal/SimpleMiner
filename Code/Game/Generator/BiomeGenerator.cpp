#include "BiomeGenerator.h"

#include "ThirdParty/Noise/SmoothNoise.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Game/Gamecommon.hpp"

extern Game* g_theGame;

BiomeGenerator::BiomeGenerator(unsigned int baseSeed)
{
    m_temperatureSeed = baseSeed + 0;
    m_humiditySeed = baseSeed + 1;
    m_continentalSeed = baseSeed + 2;
    m_erosionSeed = baseSeed + 3;
    m_weirdnessSeed = baseSeed + 4;
    m_peaksValleysSeed = baseSeed + 5;
}

BiomeGenerator::~BiomeGenerator()
{
}

ContinentalnessType BiomeGenerator::ClassifyContinentalness(float c)
{
    if (c < -1.05f)
        return CONT_DEEP_OCEAN;  
    else if (c < -0.455f)
        return CONT_DEEP_OCEAN;
    else if (c < -0.19f)
        return CONT_OCEAN;
    else if (c < -0.11f)
        return CONT_COAST;
    else if (c < 0.03f)
        return CONT_NEAR_INLAND;
    else if (c < 0.30f)
        return CONT_MID_INLAND;
    else
        return CONT_FAR_INLAND;
}

ErosionType BiomeGenerator::ClassifyErosion(float e)
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

PVType BiomeGenerator::ClassifyPV(float pv)
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

TemperatureType BiomeGenerator::ClassifyTemperature(float t)
{
    if (t < -0.45f)
        return T0;
    else if (t < -0.15f)
        return T1;
    else if (t < 0.20f)
        return T2;
    else if (t < 0.55f)
        return T3;
    else
        return T4;
}

HumidityType BiomeGenerator::ClassifyHumidity(float h)
{
    if (h < -0.35f)
        return H0;
    else if (h < -0.10f)
        return H1;
    else if (h < 0.10f)
        return H2;
    else if (h < 0.30f)
        return H3;
    else
        return H4;
}

BiomeGenerator::BiomeType BiomeGenerator::GetMiddleBiome(TemperatureType t, HumidityType h)
{
    // T/H    T=0              T=1         T=2         T=3         T=4
    // H=0    Snowy Plains     Plains      Plains      Savanna     Desert
    // H=1    Snowy Plains     Plains      Plains      Savanna     Desert
    // H=2    Snowy Taiga      Forest      Forest      Plains      Desert
    // H=3    Snowy Taiga      Taiga       Forest      Jungle      Desert
    // H=4    Taiga            Taiga       Jungle      Jungle      Desert
    
    if (t == T0)
    {
        if (h == H0 || h == H1)
            return BIOME_SNOWY_PLAINS;
        else if (h == H2 || h == H3)
            return BIOME_SNOWY_TAIGA;
        else  // H4
            return BIOME_TAIGA;
    }
    else if (t == T1)
    {
        if (h == H0 || h == H1)
            return BIOME_PLAINS;
        else if (h == H2)
            return BIOME_FOREST;
        else if (h == H3 || h == H4)
            return BIOME_TAIGA;
    }
    else if (t == T2)
    {
        if (h == H0 || h == H1)
            return BIOME_PLAINS;
        else if (h == H2 || h == H3)
            return BIOME_FOREST;
        else  // H4
            return BIOME_JUNGLE;
    }
    else if (t == T3)
    {
        if (h == H0 || h == H1)
            return BIOME_SAVANNA;
        else if (h == H2)
            return BIOME_PLAINS;
        else  // H3, H4
            return BIOME_JUNGLE;
    }
    else  // T4
    {
        return BIOME_DESERT;
    }
    return BIOME_UNKNOWN;
}

BiomeGenerator::BiomeType BiomeGenerator::GetBeachBiome(TemperatureType t)
{
    // T=0    → Snowy Beach
    // T=1,2,3 → Beach
    // T=4    → Desert (作为海岸)
    
    if (t == T0)
        return BIOME_SNOWY_BEACH;
    else if (t == T4)
        return BIOME_DESERT_BEACH;  // 沙漠海岸仍然是沙滩 
    else
        return BIOME_BEACH;
}

BiomeGenerator::BiomeType BiomeGenerator::GetBadlandBiome(HumidityType h)
{
    // H=0~2   → Desert
    // H=3,4   → Savanna
    
    if (h <= H2)
        return BIOME_DESERT;
    else
        return BIOME_SAVANNA;
}

BiomeGenerator::BiomeType BiomeGenerator::DetermineInlandBiome(ContinentalnessType cont, PVType pv, ErosionType erosion,
    TemperatureType temp, HumidityType humid)
{
    // 根据Inland biomes表格的Near-inland, Mid-inland, Far-inland列
    
    // Peaks的特殊情况
    if (pv == PV_PEAKS)
    {
        if (erosion == E0)
        {
            // Peaks biome（所有inland区域）
            if (temp <= T2)
                return BIOME_SNOWY_PEAKS;
            else
                return BIOME_SNOW;  // Stony Peaks
        }
        else if (erosion == E1)
        {
            if (temp == T4)
                return GetBadlandBiome(humid);
            else if (cont == CONT_MID_INLAND || cont == CONT_FAR_INLAND)
            {
                // Mid和Far inland的Peaks+E1也可能是Peaks
                if (temp <= T2)
                    return BIOME_SNOWY_PEAKS;
                else
                    return BIOME_SNOW;
            }
            else
            {
                return GetMiddleBiome(temp, humid);
            }
        }
        else  // E2~E6
        {
            return GetMiddleBiome(temp, humid);
        }
    }
    
    // High的特殊情况
    if (pv == PV_HIGH)
    {
        if (erosion == E0)
        {
            // 在Mid和Far inland可能是Peaks
            if (cont == CONT_MID_INLAND || cont == CONT_FAR_INLAND)
            {
                if (temp <= T2)
                    return BIOME_SNOWY_PEAKS;
                else
                    return BIOME_SNOW;
            }
            else
            {
                return GetMiddleBiome(temp, humid);
            }
        }
        else if (erosion == E1 && temp == T4)
        {
            return GetBadlandBiome(humid);
        }
        else if (erosion == E3 && temp == T4)
        {
            if (cont == CONT_MID_INLAND || cont == CONT_FAR_INLAND)
                return GetBadlandBiome(humid);
            else
                return GetMiddleBiome(temp, humid);
        }
        
        // 其他情况 → Middle biomes
        return GetMiddleBiome(temp, humid);
    }
    
    // Mid的情况
    if (pv == PV_MID)
    {
        if (erosion == E0 || erosion == E1)
        {
            if (temp == T4)
                return GetBadlandBiome(humid);
        }
        else if (erosion == E2)
        {
            if (temp == T4 && (cont == CONT_MID_INLAND || cont == CONT_FAR_INLAND))
                return GetBadlandBiome(humid);
        }
        else if (erosion == E3)
        {
            if (temp == T4 && cont == CONT_FAR_INLAND)
                return GetBadlandBiome(humid);
        }
        
        return GetMiddleBiome(temp, humid);
    }
    
    // Low和Valleys的情况
    if (pv == PV_LOW || pv == PV_VALLEYS)
    {
        if (temp == T4)
        {
            if (erosion == E0 || erosion == E1)
                return GetBadlandBiome(humid);
            else if (erosion == E2 && (cont == CONT_MID_INLAND || cont == CONT_FAR_INLAND))
                return GetBadlandBiome(humid);
        }
        
        return GetMiddleBiome(temp, humid);
    }
    
    // 默认：Middle biomes
    return GetMiddleBiome(temp, humid);
}

BiomeGenerator::BiomeType BiomeGenerator::DetermineBiome(const BiomeParameters& params)
{
    ContinentalnessType cont = ClassifyContinentalness(params.m_continentalness);
    PVType pv = ClassifyPV(params.m_peaksAndValleys);
    ErosionType erosion = ClassifyErosion(params.m_erosion);
    TemperatureType temp = ClassifyTemperature(params.m_temperature);
    HumidityType humid = ClassifyHumidity(params.m_humidity);
    
    // 处理海洋
    if (cont == CONT_DEEP_OCEAN)
    {
        if (temp == T0)
            return BIOME_FROZEN_OCEAN;
        else
            return BIOME_DEEP_OCEAN;
    }
    
    if (cont == CONT_OCEAN)
    {
        if (temp == T0)
            return BIOME_FROZEN_OCEAN;
        else
            return BIOME_OCEAN;
    }
    
    if (cont == CONT_COAST)
    {
        // 根据Blog中的Coast biomes表格：
        // 大部分Coast区域应该是Beach（高度固定在64）
        // 只有特殊的PV+Erosion组合才是Middle biomes
        
        // // Valleys: 全是Beach
        // if (pv == PV_VALLEYS)
        // {
        //     return GetBeachBiome(temp);
        // }
        //
        // // Low: 大部分Beach，除了E2/E3
        // if (pv == PV_LOW)
        // {
        //     if (erosion == E2 || erosion == E3)
        //         return GetMiddleBiome(temp, humid);  // Middle biomes
        //     else
        //         return GetBeachBiome(temp);  // Beach
        // }
        //
        // // Mid: 只有E3是Middle，其他都是Beach
        // if (pv == PV_MID)
        // {
        //     if (erosion == E3)
        //         return GetMiddleBiome(temp, humid);  // Middle biomes
        //     else
        //         return GetBeachBiome(temp);  // Beach
        // }
        //
        // // High: 大部分是Middle，但接近海平面
        // if (pv == PV_HIGH)
        // {
        //     if (erosion == E0)
        //         return GetMiddleBiome(temp, humid);  // Middle biomes，稍高
        //     else
        //         return GetMiddleBiome(temp, humid);  // Middle biomes
        // }
        //
        // // Peaks: 复杂情况
        // if (pv == PV_PEAKS)
        // {
        //     if (erosion == E0)
        //     {
        //         // Peaks biomes in Coast - 这种情况很少
        //         if (temp <= T2)
        //             return BIOME_SNOWY_PEAKS;
        //         else
        //             return BIOME_SNOW;
        //     }
        //     else if (erosion == E1)
        //         return GetMiddleBiome(temp, humid);
        //     else
        //         return GetMiddleBiome(temp, humid);
        // }
        
        return GetBeachBiome(temp);
    }
    
    return DetermineInlandBiome(cont, pv, erosion, temp, humid);
}

BiomeGenerator::BiomeParameters BiomeGenerator::SampleBiomeParameters(int worldX, int worldY)
{
    BiomeParameters params;
    
    // Continent: Scale = 1024.0, Octaves = 4
    params.m_continentalness = Compute2dPerlinNoise(
        (float)worldX, (float)worldY,
        g_theGame->g_continentNoiseScale, // Scale= 1024 
        g_theGame->g_continentNoiseOctaves,        
        0.5f,     
        2.0f,     
        true,
        m_continentalSeed
    );
    
    // Temperature: Scale = 512.0, Octaves = 2
    params.m_temperature = Compute2dPerlinNoise(
        (float)worldX, (float)worldY,
        g_theGame->g_temperatureNoiseScale,   // Scale = 512
        g_theGame->g_temperatureNoiseOctaves,        // Octaves = 2
        0.5f,
        2.0f,
        true,
        m_temperatureSeed
    );
    
    // Humidity: Scale = 512.0, Octaves = 4
    params.m_humidity = Compute2dPerlinNoise(
        (float)worldX, (float)worldY,
        g_theGame->g_humidityNoiseScale,   // Scale = 512
        g_theGame->g_humidityNoiseOctaves,        // Octaves = 4
        0.5f,
        2.0f,
        true,
        m_humiditySeed
    );
    
    // Erosion: Scale = 512.0, Octaves = 8
    params.m_erosion = Compute2dPerlinNoise(
        (float)worldX, (float)worldY,
        g_theGame->g_erosionNoiseScale,   // Scale = 512
        g_theGame->g_erosionNoiseOctaves,        // Octaves = 8
        0.5f,
        2.0f,
        true,
        m_erosionSeed
    );
    
    // Weirdness: Scale = 512.0, Octaves = 4
    params.m_weirdness = Compute2dPerlinNoise(
        (float)worldX, (float)worldY,
        512.0f,   // Scale = 512
        4,        // Octaves = 4
        0.5f,
        2.0f,
        true,
        m_weirdnessSeed
    );
    
    // Peaks and Valleys: Scale = 512.0, Octaves = 8
    float rawPV = Compute2dPerlinNoise(
        (float)worldX, (float)worldY,
        g_theGame->g_humidityNoiseScale,   // Scale = 512
        g_theGame->g_peaksValleysNoiseOctaves,        // Octaves = 8
        0.5f,
        2.0f,
        true,
        m_peaksValleysSeed
    );
    
    // PV = 1 - |3|N| - 2|
    params.m_peaksAndValleys = 1.0f - fabsf(3.0f * fabsf(rawPV) - 2.0f);
    
    return params;
}
