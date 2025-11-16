#pragma once
#include "Engine/Save/ISerializable.h"

class Chunk;

#pragma pack(push, 1)
struct ChunkFileHeader
{
    char fourCC[4];      // 'GCHK'
    uint8_t version;     // 1
    uint8_t bitsX;       // CHUNK_BITS_X
    uint8_t bitsY;       // CHUNK_BITS_Y
    uint8_t bitsZ;       // CHUNK_BITS_Z
    
    ChunkFileHeader() = default;
    ChunkFileHeader(uint8_t x, uint8_t y, uint8_t z)
        : version(1), bitsX(x), bitsY(y), bitsZ(z)
    {
        fourCC[0] = 'G';
        fourCC[1] = 'C';
        fourCC[2] = 'H';
        fourCC[3] = 'K';
    }
    
    bool Validate(uint8_t expectedX, uint8_t expectedY, uint8_t expectedZ) const
    {
        return fourCC[0] == 'G' && fourCC[1] == 'C' && 
               fourCC[2] == 'H' && fourCC[3] == 'K' &&
               version == 1 &&
               bitsX == expectedX && bitsY == expectedY && bitsZ == expectedZ;
    }
};
#pragma pack(pop)

class ChunkSerializer : public ISerializable
{
    
public:
    ChunkSerializer(Chunk* myChunk);
    ~ChunkSerializer();
    virtual void SaveToBinary(std::vector<uint8_t>& buffer) const override;
    virtual bool LoadFromBinary(const std::vector<uint8_t>& buffer, size_t& offset) override;
    virtual std::string GetSaveIdentifier() const override;

public:
    Chunk* m_chunk;
    uint8_t* m_blockData = nullptr;
};
