#include "ChunkUtils.h"
#include <cmath>

int LocalCoordsToIndex(const IntVec3& localCoords)
{
    return localCoords.x | (localCoords.y << CHUNK_BITS_X) | (localCoords.z << CHUNK_BITS_XY);
}

int LocalCoordsToIndex(int x, int y, int z)
{
    return x | (y << CHUNK_BITS_X) | (z << CHUNK_BITS_XY);
}

int IndexToLocalX(int index)
{
    return index & CHUNK_MASK_X;
}

int IndexToLocalY(int index)
{
    return (index & CHUNK_MASK_Y) >> CHUNK_BITS_X;
}

int IndexToLocalZ(int index)
{
    return (index & CHUNK_MASK_Z) >> CHUNK_BITS_XY;
}

IntVec3 IndexToLocalCoords(int index)
{
    return IntVec3(
        IndexToLocalX(index),
        IndexToLocalY(index),
        IndexToLocalZ(index)
    );
}

int GlobalCoordsToIndex(const IntVec3& globalCoords)
{
    IntVec3 localCoords = GlobalCoordsToLocalCoords(globalCoords);
    return LocalCoordsToIndex(localCoords);
}

int GlobalCoordsToIndex(int x, int y, int z)
{
    return GlobalCoordsToIndex(IntVec3(x, y, z));
}

IntVec2 GetChunkCoords(const IntVec3& globalCoords)
{
    int chunkX = FloorDivision(globalCoords.x, CHUNK_SIZE_X);
    int chunkY = FloorDivision(globalCoords.y, CHUNK_SIZE_Y);
    return IntVec2(chunkX, chunkY);
}

IntVec2 GetChunkCenter(const IntVec2& chunkCoords)
{
    int centerX = (chunkCoords.x * CHUNK_SIZE_X) + (CHUNK_SIZE_X / 2);
    int centerY = (chunkCoords.y * CHUNK_SIZE_Y) + (CHUNK_SIZE_Y / 2);
    return IntVec2(centerX, centerY);
}

IntVec3 GlobalCoordsToLocalCoords(const IntVec3& globalCoords)
{
    int localX = globalCoords.x & CHUNK_MAX_X;
    int localY = globalCoords.y & CHUNK_MAX_Y;
    int localZ = globalCoords.z;  
    
    if (globalCoords.x < 0 && localX != 0)
    {
        localX = CHUNK_SIZE_X - ((-globalCoords.x) & CHUNK_MAX_X);
    }
    if (globalCoords.y < 0 && localY != 0)
    {
        localY = CHUNK_SIZE_Y - ((-globalCoords.y) & CHUNK_MAX_Y);
    }
    
    return IntVec3(localX, localY, localZ);
}

IntVec3 GetGlobalCoords(const IntVec2& chunkCoords, int blockIndex)
{
    IntVec3 localCoords = IndexToLocalCoords(blockIndex);
    return GetGlobalCoords(chunkCoords, localCoords);
}

IntVec3 GetGlobalCoords(const IntVec2& chunkCoords, const IntVec3& localCoords)
{
    int globalX = (chunkCoords.x * CHUNK_SIZE_X) + localCoords.x;
    int globalY = (chunkCoords.y * CHUNK_SIZE_Y) + localCoords.y;
    int globalZ = localCoords.z;
    
    return IntVec3(globalX, globalY, globalZ);
}

IntVec3 GetGlobalCoords(const Vec3& position)
{
    int globalX = static_cast<int>(std::floor(position.x));
    int globalY = static_cast<int>(std::floor(position.y));
    int globalZ = static_cast<int>(std::floor(position.z));
    
    return IntVec3(globalX, globalY, globalZ);
}

bool IsSolid(uint8_t type)
{
	switch (type)
	{
	case BLOCK_TYPE_STONE:
	case BLOCK_TYPE_DIRT:
	case BLOCK_TYPE_GRASS:
	case BLOCK_TYPE_SAND:
	case BLOCK_TYPE_COBBLESTONE:
	case BLOCK_TYPE_CHISELED_BRICK:
	case BLOCK_TYPE_OBSIDIAN:
	case BLOCK_TYPE_COAL:
	case BLOCK_TYPE_IRON:
	case BLOCK_TYPE_GOLD:
	case BLOCK_TYPE_DIAMOND:
	case BLOCK_TYPE_GLOWSTONE:
	case BLOCK_TYPE_ICE:
	case BLOCK_TYPE_OAK_LOG:
	case BLOCK_TYPE_BIRCH_LOG:
	case BLOCK_TYPE_CACTUS_LOG:
	case BLOCK_TYPE_SPRUCE_LOG:
	case BLOCK_TYPE_JUNGLE_LOG:
	case BLOCK_TYPE_ACACIA_LOG:
	case BLOCK_TYPE_BIRCH_PLANKS:
	case BLOCK_TYPE_OAK_PLANKS:
	case BLOCK_TYPE_JUNGLE_PLANKS:
	case BLOCK_TYPE_SPRUCE_PLANKS:
	case BLOCK_TYPE_ACACIA_PLANKS:
	case BLOCK_TYPE_SNOW:
		return true;
	default:
		return false;
	}
}

bool IsLiquid(uint8_t type)
{
	return (type == BLOCK_TYPE_WATER || type == BLOCK_TYPE_LAVA);
}

bool IsFoliage(uint8_t type)
{
	return (type == BLOCK_TYPE_BIRCH_LEAVES
        || type == BLOCK_TYPE_OAK_LEAVES
        || type == BLOCK_TYPE_ACACIA_LEAVES
        || type == BLOCK_TYPE_JUNGLE_LEAVES
        || type == BLOCK_TYPE_SPRUCE_LEAVES
        || type == BLOCK_TYPE_SPRUCE_LEAVES_SNOW);
}

bool IsLog(uint8_t type)
{
	switch (type) 
    {
	case BLOCK_TYPE_OAK_LOG:
	case BLOCK_TYPE_BIRCH_LOG:
	case BLOCK_TYPE_SPRUCE_LOG:
	case BLOCK_TYPE_JUNGLE_LOG:
	case BLOCK_TYPE_ACACIA_LOG:
	case BLOCK_TYPE_CACTUS_LOG:
		return true;
	default:
		return false;
	}
}

bool IsSnow(uint8_t type)
{
    return type == BLOCK_TYPE_SNOW;
}

bool IsNonGroundCover(uint8_t type)
{
	return (type == BLOCK_TYPE_AIR) || IsFoliage(type) || IsLiquid(type);
}
