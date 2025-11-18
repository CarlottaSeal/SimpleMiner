#pragma once
#include "Engine/UI/UIScreen.h"
#include "ItemSlot.h"
#include <vector>

class Panel;
class Text;
class ProgressBar;

class FurnaceScreen : public UIScreen
{
public:
    FurnaceScreen(UISystem* uiSystem, int furnaceID);
    virtual ~FurnaceScreen() override;
    
    virtual void Build() override;
    virtual void Update(float deltaSeconds) override;
    
    // 数据接口
    void SetInputItem(ItemData const& item);
    void SetFuelItem(ItemData const& item);
    void SetOutputItem(ItemData const& item);
    void UpdateSmeltingProgress(float progress);  // 0.0 - 1.0
    void UpdateFuelProgress(float progress);      // 0.0 - 1.0
    
private:
    int m_furnaceID = -1;
    
    static const int PLAYER_ROWS = 3;
    static const int PLAYER_COLS = 9;
    static const int PLAYER_HOTBAR = 9;
    static const int PLAYER_SLOTS = PLAYER_ROWS * PLAYER_COLS + PLAYER_HOTBAR;
    
    Panel* m_backgroundDimmer = nullptr;
    Panel* m_furnacePanel = nullptr;
    Text* m_titleText = nullptr;
    
    ItemSlot* m_inputSlot = nullptr;   // 待烧炼物品
    ItemSlot* m_fuelSlot = nullptr;    // 燃料
    ItemSlot* m_outputSlot = nullptr;  // 烧炼结果
    
    ProgressBar* m_smeltingProgressBar = nullptr;  // 烧炼进度
    ProgressBar* m_fuelProgressBar = nullptr;      // 燃料消耗进度
    
    std::vector<ItemSlot*> m_playerSlots;
    
    void BuildBackground();
    void BuildFurnaceSlots();
    void BuildProgressBars();
    void BuildPlayerGrid();
    void RegisterSlotEvents();
};