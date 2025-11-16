#include "ChunkSerializer.h"

#include "Chunk.h"
#include "Gamecommon.hpp"
#include "Engine/Save/RLECompression.h"

ChunkSerializer::ChunkSerializer(Chunk* myChunk)
    : m_chunk(myChunk)
{
    // 分配足够空间存储所有 Block 数据 (3 bytes each)
    m_blockData = new uint8_t[CHUNK_TOTAL_BLOCKS * sizeof(Block)];
    
    // 复制 Block 数据
    memcpy(m_blockData, myChunk->m_blocks, CHUNK_TOTAL_BLOCKS * sizeof(Block));
}

ChunkSerializer::~ChunkSerializer()
{
    // 释放内存
    if (m_blockData)
    {
        delete[] m_blockData;
        m_blockData = nullptr;
    }
}

void ChunkSerializer::SaveToBinary(std::vector<uint8_t>& buffer) const
{
    if (!m_chunk || !m_chunk->m_blocks)
        return;

    // 1. 写入 header
    ChunkFileHeader header(CHUNK_BITS_X, CHUNK_BITS_Y, CHUNK_BITS_Z);
    size_t headerPos = buffer.size();
    buffer.resize(headerPos + sizeof(ChunkFileHeader));
    memcpy(buffer.data() + headerPos, &header, sizeof(ChunkFileHeader));

    // 2. 准备要压缩的数据（所有 3 个 bytes per block）
    size_t totalBytes = CHUNK_TOTAL_BLOCKS * sizeof(Block);  // 总字节数
    const uint8_t* blockBytes = reinterpret_cast<const uint8_t*>(m_chunk->m_blocks);
    
    // 3. 使用 RLE 压缩所有字节
    std::vector<uint8_t> compressed = RLECompression::CompressBytes(blockBytes, totalBytes);
    
    // 4. 写入压缩后的数据
    size_t compressedPos = buffer.size();
    buffer.resize(compressedPos + compressed.size());
    memcpy(buffer.data() + compressedPos, compressed.data(), compressed.size());
}

bool ChunkSerializer::LoadFromBinary(const std::vector<uint8_t>& buffer, size_t& offset)
{
    // 1. 读取并验证 header
    if (offset + sizeof(ChunkFileHeader) > buffer.size())
    {
        ERROR_RECOVERABLE("Chunk file too small for header");
        return false;
    }

    ChunkFileHeader header;
    memcpy(&header, buffer.data() + offset, sizeof(ChunkFileHeader));
    
    if (!header.Validate(CHUNK_BITS_X, CHUNK_BITS_Y, CHUNK_BITS_Z))
    {
        ERROR_RECOVERABLE("Invalid chunk header");
        return false;
    }
    
    offset += sizeof(ChunkFileHeader);

    // 2. 计算剩余数据大小（压缩数据）
    size_t compressedSize = buffer.size() - offset;
    
    // 3. 验证压缩数据大小（RLE 格式必须是偶数，因为是 [value][count] 对）
    if (compressedSize == 0 || compressedSize % 2 != 0)
    {
        ERROR_RECOVERABLE("Invalid RLE data size");
        return false;
    }

    // 4. 确保 m_blockData 已分配
    if (!m_blockData)
    {
        m_blockData = new uint8_t[CHUNK_TOTAL_BLOCKS * sizeof(Block)];
    }
    
    // 5. 解压缩数据
    size_t totalBytes = CHUNK_TOTAL_BLOCKS * sizeof(Block);
    bool success = RLECompression::DecompressBytes(
        buffer.data() + offset, 
        compressedSize, 
        m_blockData, 
        totalBytes  // 期望解压后的字节数
    );
    
    if (!success)
    {
        ERROR_RECOVERABLE("RLE decompression failed");
        return false;
    }
    
    // 6. 复制解压后的数据到 chunk->m_blocks
    memcpy(m_chunk->m_blocks, m_blockData, totalBytes);
    
    offset += compressedSize;
    return true;
}

std::string ChunkSerializer::GetSaveIdentifier() const
{
    return "GCHK";
}