#pragma once
#include <map>
#include <vector>

#include "BiomeGenerator.h"
#include "Engine/Math/RandomNumberGenerator.hpp"

class Chunk;
enum BlockType : uint8_t;
struct IntVec3;

class FeaturePlacer
{
public:
    struct TreeStamp
    {
        std::vector<IntVec3> m_logPositions;
        std::vector<IntVec3> m_leafPositions;
        BlockType m_logType;
        BlockType m_leafType;
    };

    FeaturePlacer(unsigned int baseSeed);
    
    bool ShouldPlaceTree(int worldX, int worldY, BiomeGenerator::BiomeType biome);
    
    void PlaceTree(Chunk* chunk, int localX, int localY, int surfaceZ, 
                  BiomeGenerator::BiomeType biome);
    void PlaceTree(Chunk* chunk, int localX, int localY, int surfaceZ, 
                   BiomeGenerator::BiomeType biome, float temperature);
    
    TreeStamp GetTreeStamp(BiomeGenerator::BiomeType biome);
    
private:
    void InitializeTreeStamps();
    
    std::vector<IntVec3> MakeColumn(int height);
    std::vector<IntVec3> MakeBlobLeaves(const IntVec3& center, int radius);
    std::vector<IntVec3> MakeSpruceLeaves(int treeHeight);
    std::vector<IntVec3> MakeAcaciaLeaves();
    std::vector<IntVec3> MakeDarkOakTrunk(int height);
    
    bool CanPlaceTree(Chunk* chunk, int localX, int localY, int surfaceZ, 
                     const TreeStamp& stamp);
    
    std::vector<TreeStamp> m_oakVariants;
    std::vector<TreeStamp> m_birchVariants;
    std::vector<TreeStamp> m_spruceVariants;
    std::vector<TreeStamp> m_snowySpruceVariants;
    std::vector<TreeStamp> m_jungleVariants;
    std::vector<TreeStamp> m_acaciaVariants;
    std::vector<TreeStamp> m_darkOakVariants;
    std::vector<TreeStamp> m_cactusVariants;
    
    std::map<BiomeGenerator::BiomeType, std::vector<TreeStamp>> m_treeStamps;
    
    unsigned int m_treeSeed;
    RandomNumberGenerator m_rng;
};