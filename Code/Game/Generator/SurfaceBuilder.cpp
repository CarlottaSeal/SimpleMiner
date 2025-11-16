#include "SurfaceBuilder.h"

#include "Game/Block.h"
#include "Game/ChunkUtils.h"
#include "Game/Game.hpp"
#include "ThirdParty/Noise/SmoothNoise.hpp"

extern Game* g_theGame;
SurfaceBuilder::SurfaceBuilder()
{
}

SurfaceBuilder::SurfaceConfig SurfaceBuilder::GetSurfaceConfig(
    BiomeGenerator::BiomeType biome, 
    float temperature,
    float humidity)
{
    SurfaceConfig config;
    
    switch (biome) 
    {
        case BiomeGenerator::BIOME_DESERT:
        case BiomeGenerator::BIOME_DESERT_BEACH:
            config.m_topBlock = BLOCK_TYPE_SAND;
            config.m_fillerBlock = BLOCK_TYPE_SAND;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 3;
            config.m_underDepth = 4;
            break;
            
        // ========== 海滩生物群系 ==========
        case BiomeGenerator::BIOME_BEACH:
            config.m_topBlock = BLOCK_TYPE_GRASS;
            config.m_fillerBlock = BLOCK_TYPE_SAND;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 2;
            config.m_underDepth = 3;
            break;
            
        case BiomeGenerator::BIOME_SNOWY_BEACH:
            config.m_topBlock = BLOCK_TYPE_SNOW;
            config.m_fillerBlock = BLOCK_TYPE_SAND;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 2;
            config.m_underDepth = 0;
            break;
            
        case BiomeGenerator::BIOME_OCEAN:
            config.m_topBlock = BLOCK_TYPE_SAND;
            config.m_fillerBlock = BLOCK_TYPE_SAND;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 2;
            config.m_underDepth = 0;
            break;
            
        case BiomeGenerator::BIOME_FROZEN_OCEAN:
            config.m_topBlock = BLOCK_TYPE_SAND;
            config.m_fillerBlock = BLOCK_TYPE_SAND;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 1;
            config.m_underDepth = 0;
            break;
            
        case BiomeGenerator::BIOME_DEEP_OCEAN:
            config.m_topBlock = BLOCK_TYPE_SAND;
            config.m_fillerBlock = BLOCK_TYPE_STONE;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 1;
            config.m_underDepth = 0;
            break;
            
        case BiomeGenerator::BIOME_SNOWY_PLAINS:
        case BiomeGenerator::BIOME_SNOWY_TAIGA:
            config.m_topBlock = BLOCK_TYPE_SNOW;
            config.m_fillerBlock = BLOCK_TYPE_DIRT;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 3;
            config.m_underDepth = 0;
            break;
            
        case BiomeGenerator::BIOME_SNOWY_PEAKS:
        case BiomeGenerator::BIOME_SNOW:
            config.m_topBlock = BLOCK_TYPE_SNOW;
            config.m_fillerBlock = BLOCK_TYPE_SNOW;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 2;
            config.m_fillerDepth = 3;
            config.m_underDepth = 0;
            break;
            
        // ========== 森林/平原/草原 ==========
        case BiomeGenerator::BIOME_FOREST:
        case BiomeGenerator::BIOME_PLAINS:
            config.m_topBlock = BLOCK_TYPE_GRASS;
            config.m_fillerBlock = BLOCK_TYPE_DIRT;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 3;
            config.m_underDepth = 0;
            break;
            
        case BiomeGenerator::BIOME_TAIGA:
            config.m_topBlock = BLOCK_TYPE_GRASS;
            config.m_fillerBlock = BLOCK_TYPE_DIRT;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 2;
            config.m_underDepth = 0;
            break;
            
        case BiomeGenerator::BIOME_JUNGLE:
            config.m_topBlock = BLOCK_TYPE_GRASS;
            config.m_fillerBlock = BLOCK_TYPE_DIRT;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 4;  
            config.m_underDepth = 0;
            break;
            
        case BiomeGenerator::BIOME_SAVANNA:
            if (humidity < 0.3f)
            {
                config.m_topBlock = BLOCK_TYPE_SAND;
                config.m_fillerBlock = BLOCK_TYPE_SAND;
                config.m_underBlock = BLOCK_TYPE_STONE;
            }
            else
            {
                config.m_topBlock = BLOCK_TYPE_GRASS;
                config.m_fillerBlock = BLOCK_TYPE_DIRT;
                config.m_underBlock = BLOCK_TYPE_STONE;
            }
            config.m_topDepth = 1;
            config.m_fillerDepth = 2;
            config.m_underDepth = 2;
            break;
            
        // ========== 沼泽 ==========
        case BiomeGenerator::BIOME_SWAMP:
            if (humidity > 0.7f)
            {
                config.m_topBlock = BLOCK_TYPE_DIRT;  // 泥泞的表面
            }
            else
            {
                config.m_topBlock = BLOCK_TYPE_GRASS;
            }
            config.m_fillerBlock = BLOCK_TYPE_DIRT;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 3;
            config.m_underDepth = 0;
            break;
            
        // ========== 默认 ==========
        default:
            config.m_topBlock = BLOCK_TYPE_GRASS;
            config.m_fillerBlock = BLOCK_TYPE_DIRT;
            config.m_underBlock = BLOCK_TYPE_STONE;
            config.m_topDepth = 1;
            config.m_fillerDepth = 3;
            config.m_underDepth = 0;
            break;
    }
    
    // 温度覆盖（适用于所有草地）
    if (temperature < -0.5f && config.m_topBlock == BLOCK_TYPE_GRASS)
    {
        config.m_topBlock = BLOCK_TYPE_SNOW;
    }
    
    return config;
}

void SurfaceBuilder::BuildSurface(
    Block* blocks, 
    int localX, 
    int localY, 
    int surfaceHeight, 
    const SurfaceConfig& config)
{
    if (surfaceHeight < 0 || surfaceHeight >= CHUNK_SIZE_Z)
    {
        return;
    }
    
    // 从表面向下处理
    int currentDepth = 0;
    
    for (int z = surfaceHeight; z >= 0 && currentDepth < 20; z--)
    {
        int idx = LocalCoordsToIndex(localX, localY, z);
        uint8_t blockType = blocks[idx].m_typeIndex;
        
        // 跳过空气和水
        if (blockType == BLOCK_TYPE_AIR || blockType == BLOCK_TYPE_WATER)
        {
            continue;
        }
        
        // 只替换石头
        if (blockType != BLOCK_TYPE_STONE)
        {
            break;
        }
        
        // 根据深度决定方块类型
        if (currentDepth < config.m_topDepth)
        {
            // 表层
            blocks[idx].SetType(config.m_topBlock);
        }
        else if (currentDepth < config.m_topDepth + config.m_fillerDepth)
        {
            // 填充层
            blocks[idx].SetType( config.m_underBlock);
        }
        else
        {
            // 更深就是石头，停止
            break;
        }
        
        currentDepth++;
    }
}

void SurfaceBuilder::BuildSurface(
    Block* blocks, 
    int localX, 
    int localY, 
    const SurfaceConfig& config)
{
    // 从上往下找第一个固体方块
    int surfaceZ = -1;
    
    for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--)
    {
        int idx = LocalCoordsToIndex(localX, localY, z);
        uint8_t blockType = blocks[idx].m_typeIndex;
        
        if (blockType != BLOCK_TYPE_AIR && blockType != BLOCK_TYPE_WATER)
        {
            surfaceZ = z;
            break;
        }
    }
    
    if (surfaceZ >= 0)
    {
        BuildSurface(blocks, localX, localY, surfaceZ, config);
    }
}

// ========================================
// 温度覆盖（冻结水面等）
// ========================================
void SurfaceBuilder::ApplyTemperatureOverrides(Block* blocks, float temperature)
{
    // 极寒温度下，水面结冰
    if (temperature < -0.6f)
    {
        for (int y = 0; y < CHUNK_SIZE_Y; y++)
        {
            for (int x = 0; x < CHUNK_SIZE_X; x++)
            {
                for (int z = g_theGame->g_seaLevel + 2; z >= g_theGame->g_seaLevel - 2; z--)
                {
                    int idx = LocalCoordsToIndex(x, y, z);
                    
                    if (blocks[idx].m_typeIndex == BLOCK_TYPE_WATER)
                    {
                        if (z + 1 < CHUNK_SIZE_Z)
                        {
                            int idxUp = LocalCoordsToIndex(x, y, z + 1);
                            if (blocks[idxUp].m_typeIndex == BLOCK_TYPE_AIR)
                            {
                                blocks[idx].SetType(BLOCK_TYPE_ICE);
                                break;  // 只冻结最上层
                            }
                        }
                    }
                }
            }
        }
    }
}

void SurfaceBuilder::GenerateOres(Block* blocks, const IntVec2& chunkCoords)
{
    for (int z = 2; z < CHUNK_SIZE_Z; z++)
    {
        for (int y = 0; y < CHUNK_SIZE_Y; y++)
        {
            for (int x = 0; x < CHUNK_SIZE_X; x++)
            {
                int idx = LocalCoordsToIndex(x, y, z);
                
                if (blocks[idx].m_typeIndex != BLOCK_TYPE_STONE)
                    continue;
                
                int worldX = chunkCoords.x * CHUNK_SIZE_X + x;
                int worldY = chunkCoords.y * CHUNK_SIZE_Y + y;
                
                float oreNoise = Compute3dPerlinNoise(
                    (float)worldX, (float)worldY, (float)z,
                    16.0f,  
                    2,     
                    0.5f,
                    2.0f,
                    false,
                    m_oreSeed
                );
                
                if (z <= 16 && oreNoise > 0.95f)
                {
                    blocks[idx].SetType(BLOCK_TYPE_DIAMOND);
                }
                else if (z <= 32 && oreNoise > 0.9f)
                {
                    blocks[idx].SetType(BLOCK_TYPE_GOLD);
                }
                else if (z <= 64 && oreNoise > 0.75f)
                {
                    blocks[idx].SetType(BLOCK_TYPE_IRON);
                }
                else if (oreNoise > 0.65f)
                {
                    blocks[idx].SetType(BLOCK_TYPE_COAL);
                }
            }
        }
    }
}