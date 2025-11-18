#include "ItemSlot.h"
#include "Engine/UI/Canvas.hpp"
#include "Engine/UI/Panel.h"
#include "Engine/UI/Sprite.h"
#include "Engine/UI/Text.h"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/VertexUtils.hpp"

ItemSlot::ItemSlot(Canvas* canvas, AABB2 const& bounds, int slotIndex)
    : m_canvas(canvas)
    , m_slotIndex(slotIndex)
{
    m_bound = bounds;
    m_type = PANEL;
    
    if (m_canvas)
    {
        m_canvas->AddElementToCanvas(this);
    }
}

ItemSlot::~ItemSlot()
{
    ShutDown();
}

void ItemSlot::StartUp()
{
    // 背景槽位
    m_background = new Panel(
        m_canvas,
        m_bound,
        Rgba8(55, 55, 55),      // 深灰色背景
        nullptr,
        true,
        Rgba8(139, 139, 139)    // 边框
    );
    
    m_itemIcon = new Sprite(m_canvas, m_bound, nullptr);
    m_itemIcon->SetEnabled(false);
    
    // 物品数量文本（右下角）
    TextSetting countSetting;
    countSetting.m_text = "";
    countSetting.m_color = Rgba8::WHITE;
    countSetting.m_height = 16.0f; //TODO: 适配
    
    Vec2 countPos = m_bound.GetBottomRight() + Vec2(-8, 4);
    m_countText = new Text(m_canvas, countPos, countSetting);
    m_countText->SetEnabled(false);
    
    // 高亮框
    m_highlightFrame = new Panel(
        m_canvas,
        m_bound,
        Rgba8(255, 255, 255, 50),
        nullptr,
        false,
        Rgba8::WHITE
    );
    m_highlightFrame->SetEnabled(false);
    
    // 选中框（被点击选中）
    m_selectionFrame = new Panel(
        m_canvas,
        m_bound,
        Rgba8(255, 255, 255, 0),   // 透明背景
        nullptr,
        true,
        Rgba8(255, 255, 0)         // 黄色边框
    );
    m_selectionFrame->SetEnabled(false);
}

void ItemSlot::ShutDown()
{
    if (m_background) { delete m_background; m_background = nullptr; }
    if (m_itemIcon) { delete m_itemIcon; m_itemIcon = nullptr; }
    if (m_countText) { delete m_countText; m_countText = nullptr; }
    if (m_highlightFrame) { delete m_highlightFrame; m_highlightFrame = nullptr; }
    if (m_selectionFrame) { delete m_selectionFrame; m_selectionFrame = nullptr; }
}

void ItemSlot::Update()
{
    if (!IsEnabled() || m_isDisabled)
    {
        return;
    }
    
    HandleInput();
}

void ItemSlot::Render() const
{
    if (!IsEnabled())
    {
        return;
    }
    
    // 渲染顺序：背景 -> 图标 -> 数量 -> 高亮/选中框
    if (m_background) 
        m_background->Render();
    
    if (m_itemIcon && !IsEmpty()) 
        m_itemIcon->Render();
    
    if (m_countText && m_itemData.count > 1) 
        m_countText->Render();
    
    if (m_selectionFrame && m_isSelected) 
        m_selectionFrame->Render();
    
    if (m_highlightFrame && m_isHighlighted) 
        m_highlightFrame->Render();
}

void ItemSlot::SetItem(ItemData const& item)
{
    m_itemData = item;
    UpdateVisuals();
}

void ItemSlot::ClearItem()
{
    m_itemData = ItemData();
    UpdateVisuals();
}

ItemData ItemSlot::TakeItem()
{
    ItemData taken = m_itemData;
    ClearItem();
    return taken;
}

int ItemSlot::AddItem(ItemData const& item)
{
    if (IsEmpty())
    {
        // 槽位为空，直接放入
        int toAdd = (item.count <= item.maxStack) ? item.count : item.maxStack;
        m_itemData = item;
        m_itemData.count = toAdd;
        UpdateVisuals();
        return item.count - toAdd;
    }
    else if (m_itemData.CanStackWith(item))
    {
        // 可以堆叠
        int spaceLeft = m_itemData.maxStack - m_itemData.count;
        int toAdd = (item.count <= spaceLeft) ? item.count : spaceLeft;
        m_itemData.count += toAdd;
        UpdateVisuals();
        return item.count - toAdd;
    }
    
    // 不能添加
    return item.count;
}

void ItemSlot::SetHighlighted(bool highlighted)
{
    m_isHighlighted = highlighted;
    if (m_highlightFrame)
    {
        m_highlightFrame->SetEnabled(highlighted);
    }
}

void ItemSlot::SetSelected(bool selected)
{
    m_isSelected = selected;
    if (m_selectionFrame)
    {
        m_selectionFrame->SetEnabled(selected);
    }
}

void ItemSlot::SetDisabled(bool disabled)
{
    m_isDisabled = disabled;
    
    if (m_background)
    {
        if (disabled)
        {
            m_background->SetBackgroundColor(Rgba8(30, 30, 30));  // 更暗
        }
        else
        {
            m_background->SetBackgroundColor(Rgba8(55, 55, 55));
        }
    }
}

size_t ItemSlot::OnClickEvent(UICallbackFunctionPointer callback)
{
    return m_onClickEvent.AddListener(callback);
}

size_t ItemSlot::OnRightClickEvent(UICallbackFunctionPointer callback)
{
    return m_onRightClickEvent.AddListener(callback);
}

size_t ItemSlot::OnHoverEnterEvent(UICallbackFunctionPointer callback)
{
    return m_onHoverEnterEvent.AddListener(callback);
}

size_t ItemSlot::OnHoverExitEvent(UICallbackFunctionPointer callback)
{
    return m_onHoverExitEvent.AddListener(callback);
}

void ItemSlot::HandleInput()
{
    if (!m_canvas)
    {
        return;
    }
    
    InputSystem* input = m_canvas->GetSystemInputSystem();
    if (!input)
    {
        return;
    }
    
    Vec2 mousePos = input->GetCursorClientPosition();
    bool isHovered = m_bound.IsPointInside(mousePos);
    
    // 悬停状态变化
    if (isHovered != m_isHovered)
    {
        if (isHovered)
        {
            SetHighlighted(true);
            m_onHoverEnterEvent.Invoke();
        }
        else
        {
            SetHighlighted(false);
            m_onHoverExitEvent.Invoke();
        }
        m_isHovered = isHovered;
    }
    
    // 左键点击
    if (isHovered && input->WasKeyJustPressed(KEYCODE_LEFT_MOUSE))
    {
        m_onClickEvent.Invoke();
    }
    
    // 右键点击
    if (isHovered && input->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE))
    {
        m_onRightClickEvent.Invoke();
    }
}

void ItemSlot::UpdateVisuals()
{
    // 更新图标
    if (m_itemIcon)
    {
        m_itemIcon->SetTexture(m_itemData.texture);
        m_itemIcon->SetEnabled(!IsEmpty());
    }
    
    // 更新数量文本
    if (m_countText)
    {
        if (m_itemData.count > 1)
        {
            m_countText->SetText(std::to_string(m_itemData.count));
            m_countText->SetEnabled(true);
        }
        else
        {
            m_countText->SetEnabled(false);
        }
    }
}