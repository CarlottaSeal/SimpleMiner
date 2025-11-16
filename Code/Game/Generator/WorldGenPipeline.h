#pragma once
#include "BiomeGenerator.h"
#include "CaveGenerator.h"
#include "FeaturePlacer.h"
#include "SurfaceBuilder.h"
#include "TerrainGenerator.h"

class Chunk;

struct ChunkGenData
{
	BiomeGenerator::BiomeType m_biomes[CHUNK_SIZE_X][CHUNK_SIZE_Y];
	int m_surfaceHeights[CHUNK_SIZE_X][CHUNK_SIZE_Y];
	BiomeGenerator::BiomeParameters m_biomeParams[CHUNK_SIZE_X][CHUNK_SIZE_Y];
};

class WorldGenPipeline
{
	enum PipelineStage
	{
		STAGE_BIOMES,  
		STAGE_NOISE,   
		STAGE_SURFACE, 
		STAGE_FEATURES,
		STAGE_CAVES,   
		STAGE_CARVERS  
	};

public:
    WorldGenPipeline();
    void GenerateChunk(Chunk* chunk);
    
private:
    void ExecuteBiomeStage(Chunk* chunk, ChunkGenData* chunkGenData);
    void ExecuteNoiseStage(Chunk* chunk, ChunkGenData* chunkGenData);
    void ExecuteSurfaceStage(Chunk* chunk, ChunkGenData* chunkGenData);
    void ExecuteFeatureStage(Chunk* chunk, ChunkGenData* chunkGenData);
    void ExecuteWaterStage(Chunk* chunk, ChunkGenData* chunkGenData);
    void ExecuteCaveStage(Chunk* chunk, ChunkGenData* chunkGenData);
    void ExecuteCarverStage(Chunk* chunk, ChunkGenData* chunkGenData);
	float GetZBias(int z);

    BiomeGenerator m_biomeGen;
    TerrainGenerator m_terrainGen;
    CaveGenerator m_caveGen;
    SurfaceBuilder m_surfaceBuilder;
    FeaturePlacer m_featurePlacer;
};
