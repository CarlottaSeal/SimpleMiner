#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"

class Block;
struct ChunkGenData;

enum CaveType
{
    CAVE_TYPE_NONE = 0,
    CAVE_TYPE_CHEESE = 1,
    CAVE_TYPE_SPAGHETTI = 2,
    CAVE_TYPE_NOODLE = 3
};

class CaveGenerator
{
public:
    CaveGenerator(unsigned int seed);
    
    // 在chunk中雕刻洞穴
    void CarveCaves(Block* blocks, const IntVec2& chunkCoords, const ChunkGenData& chunkGenData);
    uint8_t DetermineCaveFill(const Vec3& worldPos, float terrainHeight, float seaLevel, float caveness);
    void PostProcessLiquids(Block* blocks, const IntVec2& chunkCoords);

private:
    bool IsInCave(const Vec3& worldPos,
                  int distanceToSurface,
                  float terrainHeight,
                  float* outCaveness = nullptr,CaveType* outDominantType = {});
    
    int CalculateDistanceToSurface(Block* blocks, int x, int y, int z);
    
private:
    unsigned int m_cheeseSeed;      // Cheese大空洞种子
    unsigned int m_spaghettiSeed;   // Spaghetti隧道种子
    unsigned int m_noodleSeed;      // Noodle细通道种子
    unsigned int m_densitySeed;     // 洞穴密度种子
};