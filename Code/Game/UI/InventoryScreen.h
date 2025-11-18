#pragma once
#include "Engine/UI/UIScreen.h"
#include "ItemSlot.h"
#include <vector>

class Panel;
class Text;

struct DraggedItem
{
    ItemData item;
    int sourceSlotIndex = -1;
    bool isDragging = false;
};

class InventoryScreen : public UIScreen
{
public:
    InventoryScreen(UISystem* uiSystem);
    virtual ~InventoryScreen() override;
    
    virtual void Build() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void OnEnter() override;
    virtual void OnExit() override;
    
    // 物品管理
    void SetItemSlot(int slotIndex, ItemData const& item);
    ItemData GetItemSlot(int slotIndex) const;
    bool AddItemToInventory(ItemData const& item);  // 自动找空位添加
    
private:
    static const int INVENTORY_ROWS = 3;
    static const int INVENTORY_COLS = 9;
    static const int HOTBAR_SLOTS = 9;
    static const int TOTAL_SLOTS = INVENTORY_ROWS * INVENTORY_COLS + HOTBAR_SLOTS;
    
    // UI 组件
    Panel* m_backgroundDimmer = nullptr;  // 全屏半透明遮罩
    Panel* m_inventoryPanel = nullptr;
    Text* m_titleText = nullptr;
    Text* m_tooltipText = nullptr;  // 物品提示
    
    std::vector<ItemSlot*> m_itemSlots;
    
    // 拖拽状态
    DraggedItem m_draggedItem;
    Sprite* m_draggedItemSprite = nullptr;  // 跟随鼠标的物品图标
    
    // 构建方法
    void BuildBackground();
    void BuildInventoryGrid();
    void BuildTitle();
    void BuildDraggedItemSprite();
    
    // 事件注册
    void RegisterSlotEvents();
    
    // 槽位交互
    void OnSlotLeftClicked(int slotIndex);
    void OnSlotRightClicked(int slotIndex);
    void OnSlotHoverEnter(int slotIndex);
    void OnSlotHoverExit(int slotIndex);
    
    // 拖拽逻辑
    void StartDragging(int slotIndex);
    void StopDragging(int targetSlotIndex);
    void UpdateDraggedItemPosition();
    
    // 工具提示
    void ShowTooltip(ItemData const& item, Vec2 const& position);
    void HideTooltip();
    
    // 辅助方法
    int FindEmptySlot() const;
    bool IsValidSlotIndex(int index) const;
};