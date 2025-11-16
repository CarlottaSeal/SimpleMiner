#pragma once
#include "BiomeGenerator.h"
#include "Game/Gamecommon.hpp"

class Block;

class SurfaceBuilder
{
public:
    struct SurfaceConfig
    {
        uint8_t m_topBlock = BLOCK_TYPE_GRASS;      
        uint8_t m_fillerBlock = BLOCK_TYPE_DIRT;    
        uint8_t m_underBlock = BLOCK_TYPE_STONE;    // 底层方块（可选）
        int m_topDepth = 1;                         
        int m_fillerDepth = 3;                      
        int m_underDepth = 0;                       // 底层深度（0表示不使用）
    };

public:
    SurfaceBuilder();
    
    SurfaceConfig GetSurfaceConfig(BiomeGenerator::BiomeType biome, float temperature, float humidity);
    
    void BuildSurface(Block* blocks, int localX, int localY, int surfaceHeight, const SurfaceConfig& config);
    void BuildSurface(Block* blocks, int localX, int localY, const SurfaceConfig& config);
    
    void ApplyTemperatureOverrides(Block* blocks, float temperature);
    
    void GenerateOres(Block* blocks, const IntVec2& chunkCoords);

private:
    unsigned int m_oreSeed = 12345;
};