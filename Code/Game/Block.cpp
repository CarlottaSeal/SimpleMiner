#include "Block.h"

// uint8_t Block::GetOutdoorLight() const
// {
//     return (m_lightData & LIGHT_MASK_OUTDOOR) >> 4;
// }

void Block::SetOutdoorLight(uint8_t level)
{
    level = (level > 15) ? 15 : level;  // 限制在0-15
    m_lightData = (m_lightData & LIGHT_MASK_INDOOR) | (level << 4);
}

// uint8_t Block::GetIndoorLight() const
// {
//     return m_lightData & LIGHT_MASK_INDOOR;
// }

void Block::SetIndoorLight(uint8_t level)
{
    level = (level > 15) ? 15 : level;
    m_lightData = (m_lightData & LIGHT_MASK_OUTDOOR) | level;
}

bool Block::IsSky() const

{
    return (m_flags & BLOCK_BIT_IS_SKY) != 0;
}

void Block::SetIsSky(bool isSky)
{
    if (isSky)
        m_flags |= BLOCK_BIT_IS_SKY;
    else
        m_flags &= ~BLOCK_BIT_IS_SKY;
}

bool Block::IsLightDirty() const
{
    return (m_flags & BLOCK_BIT_IS_LIGHT_DIRTY) != 0;
}

void Block::SetLightDirty(bool isDirty)
{
    if (isDirty)
        m_flags |= BLOCK_BIT_IS_LIGHT_DIRTY;
    else
        m_flags &= ~BLOCK_BIT_IS_LIGHT_DIRTY;
}

// bool Block::IsOpaque() const
// {
//     return (m_flags & BLOCK_BIT_IS_OPAQUE) != 0;
// }

void Block::SetIsOpaque(bool isOpaque)
{
    if (isOpaque)
        m_flags |= BLOCK_BIT_IS_OPAQUE;
    else
        m_flags &= ~BLOCK_BIT_IS_OPAQUE;
}

bool Block::IsSolid() const
{
    return (m_flags & BLOCK_BIT_IS_SOLID) != 0;
}

void Block::SetIsSolid(bool isSolid)
{
    if (isSolid)
        m_flags |= BLOCK_BIT_IS_SOLID;
    else
        m_flags &= ~BLOCK_BIT_IS_SOLID;
}

bool Block::IsVisible() const
{
    return (m_flags & BLOCK_BIT_IS_VISIBLE) != 0;
}

void Block::SetIsVisible(bool isVisible)
{
    if (isVisible)
        m_flags |= BLOCK_BIT_IS_VISIBLE;
    else
        m_flags &= ~BLOCK_BIT_IS_VISIBLE;
}
    
