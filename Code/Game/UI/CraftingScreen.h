#pragma once
#include "Engine/UI/UIScreen.h"
#include "ItemSlot.h"
#include <vector>

class Panel;
class Text;
class Button;

struct CraftingRecipe
{
    int recipeID;
    ItemData result;
    ItemData ingredients[9];  // 3x3 合成网格
    bool shapeless = false;  // 是否无序合成
};

class CraftingScreen : public UIScreen
{
public:
    CraftingScreen(UISystem* uiSystem);
    virtual ~CraftingScreen() override;
    
    virtual void Build() override;
    virtual void Update(float deltaSeconds) override;
    
private:
    static const int CRAFTING_GRID_SIZE = 3;  // 3x3
    static const int CRAFTING_SLOTS = 9;
    static const int PLAYER_ROWS = 3;
    static const int PLAYER_COLS = 9;
    static const int PLAYER_HOTBAR = 9;
    static const int PLAYER_SLOTS = PLAYER_ROWS * PLAYER_COLS + PLAYER_HOTBAR;
    
    Panel* m_backgroundDimmer = nullptr;
    Panel* m_craftingPanel = nullptr;
    Text* m_titleText = nullptr;
    
    std::vector<ItemSlot*> m_craftingSlots;  // 3x3 合成网格
    ItemSlot* m_resultSlot = nullptr;        // 合成结果槽
    std::vector<ItemSlot*> m_playerSlots;    // 玩家背包
    
    Button* m_craftButton = nullptr;
    
    void BuildBackground();
    void BuildCraftingGrid();
    void BuildResultSlot();
    void BuildPlayerGrid();
    void RegisterSlotEvents();
    
    void OnCraftButtonClicked();
    void CheckRecipe();  // 检查当前配方
    void CraftItem();    // 执行合成
};