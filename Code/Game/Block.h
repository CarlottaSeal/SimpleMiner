#pragma once
#include <cstdint>

#include "BlockDefinition.h"
#include "Gamecommon.hpp"

class Chunk;

class Block
{
public:
    uint8_t m_typeIndex = 0;
    uint8_t m_lightData = 0;  // 高4位=室外光，低4位=室内光
    
    uint8_t m_flags = 0;

    inline void SetType(uint8_t typeIndex)
    {
        m_typeIndex = typeIndex;
        
        const BlockDefinition& def = BlockDefinition::GetBlockDef(typeIndex);
        
        m_flags &= (BLOCK_BIT_IS_SKY | BLOCK_BIT_IS_LIGHT_DIRTY);
        
        // 设置新的flags
        if (def.m_isOpaque)
            m_flags |= BLOCK_BIT_IS_OPAQUE;
        if (def.m_isSolid)
            m_flags |= BLOCK_BIT_IS_SOLID;
        if (def.m_isVisible)
            m_flags |= BLOCK_BIT_IS_VISIBLE;
    }
    
    //uint8_t GetOutdoorLight() const;
    void SetOutdoorLight(uint8_t level);
    //uint8_t GetIndoorLight() const;
    void SetIndoorLight(uint8_t level);

    inline bool IsOpaque() const
    {
        return (m_flags & BLOCK_BIT_IS_OPAQUE) != 0;
    }
    inline uint8_t GetOutdoorLight() const
    {
        return (m_lightData >> 4) & 0x0F;
    }
    inline uint8_t GetIndoorLight() const
    {
        return m_lightData & 0x0F;
    }
    
    bool IsSky() const;
    void SetIsSky(bool isSky);
    bool IsLightDirty() const;
    void SetLightDirty(bool isDirty);
    //bool IsOpaque() const;
    void SetIsOpaque(bool isOpaque);
    bool IsSolid() const;
    void SetIsSolid(bool isSolid);
    bool IsVisible() const;
    void SetIsVisible(bool isVisible);
};

static_assert(sizeof(Block) == 3, "Block should be exactly 3 bytes!");
