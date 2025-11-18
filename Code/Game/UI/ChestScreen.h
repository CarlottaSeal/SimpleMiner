#pragma once
#include "Engine/UI/UIScreen.h"
#include "ItemSlot.h"
#include <vector>

#include "InventoryScreen.h"

class Panel;
class Text;

class ChestScreen : public UIScreen
{
public:
    ChestScreen(UISystem* uiSystem, int chestID);
    virtual ~ChestScreen() override;
    
    virtual void Build() override;
    virtual void Update(float deltaSeconds) override;
    virtual void OnEnter() override;
    virtual void OnExit() override;
    

    void SetChestItem(int slotIndex, ItemData const& item);
    void SetPlayerItem(int slotIndex, ItemData const& item);
    ItemData GetChestItem(int slotIndex) const;
    ItemData GetPlayerItem(int slotIndex) const;
    
private:
    static const int CHEST_ROWS = 3;
    static const int CHEST_COLS = 9;
    static const int CHEST_SLOTS = CHEST_ROWS * CHEST_COLS;
    static const int PLAYER_ROWS = 3;
    static const int PLAYER_COLS = 9;
    static const int PLAYER_HOTBAR = 9;
    static const int PLAYER_SLOTS = PLAYER_ROWS * PLAYER_COLS + PLAYER_HOTBAR;
    
    int m_chestID = -1;
    
    Panel* m_backgroundDimmer = nullptr;
    Panel* m_chestPanel = nullptr;
    Panel* m_playerPanel = nullptr;
    Text* m_chestTitleText = nullptr;
    Text* m_inventoryTitleText = nullptr;
    
    std::vector<ItemSlot*> m_chestSlots;
    std::vector<ItemSlot*> m_playerSlots;
    
    // 拖拽状态
    DraggedItem m_draggedItem;
    Sprite* m_draggedItemSprite = nullptr;
    
    void BuildBackground();
    void BuildChestGrid();
    void BuildPlayerGrid();
    void BuildDraggedItemSprite();
    void RegisterSlotEvents();
    
    void OnChestSlotClicked(int slotIndex);
    void OnPlayerSlotClicked(int slotIndex);
    void StartDragging(int slotIndex, bool isChestSlot);
    void StopDragging(int slotIndex, bool isChestSlot);
    void UpdateDraggedItemPosition();
    
    bool IsValidChestSlot(int index) const;
    bool IsValidPlayerSlot(int index) const;
};