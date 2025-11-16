#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"

struct Block;
struct ChunkGenData;

class CaveGenerator
{
public:
    CaveGenerator(unsigned int seed);
    
    // 在chunk中雕刻洞穴
    void CarveCaves(Block* blocks, const IntVec2& chunkCoords, const ChunkGenData& chunkGenData);
    
private:
    // 判断某个位置是否在洞穴中
    // worldPos: 世界坐标
    // distanceToSurface: 到地表的距离
    // terrainHeight: 地形高度（用于海洋检测）
    // outCaveness: 输出洞穴强度（可选）
    bool IsInCave(const Vec3& worldPos, 
                  int distanceToSurface, 
                  float terrainHeight, 
                  float* outCaveness = nullptr);
    
    // 计算到地表的距离
    int CalculateDistanceToSurface(Block* blocks, int x, int y, int z);
    
    // 线性插值
    float Interpolate(float a, float b, float t);
    
private:
    unsigned int m_cheeseSeed;      // Cheese大空洞种子
    unsigned int m_spaghettiSeed;   // Spaghetti隧道种子
    unsigned int m_noodleSeed;      // Noodle细通道种子
    unsigned int m_densitySeed;     // 洞穴密度种子
};