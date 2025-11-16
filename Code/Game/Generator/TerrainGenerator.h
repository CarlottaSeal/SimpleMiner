#pragma once

#include "BiomeGenerator.h"
#include "Engine/Math/Vec3.hpp"

class Curve1D;

class TerrainGenerator
{
public:
    TerrainGenerator(unsigned int baseSeed);
    ~TerrainGenerator();
    
    float Calculate3DDensity(
        const Vec3& worldPos, 
        const BiomeGenerator::BiomeParameters& biomeParams);
    
    float FoldDensity(float density, float threshold, float strength);
    int GetSurfaceHeight(int worldX, int worldY, const BiomeGenerator::BiomeParameters& biomeParams);
    PVType ClassifyPV(float pv);
    ErosionType ClassifyErosion(float e);

private:
    BiomeGenerator m_biomeGenerator;
    unsigned int m_densitySeed;
    
    Curve1D* m_continentHeightOffsetCurve;   
    Curve1D* m_continentSquashingCurve;    
    
    Curve1D* CreateContinentHeightOffsetCurve();
    Curve1D* CreateContinentSquashingCurve();
    float GetZBias(float z);

    float GetRandomFloatZeroToOne();
};