#pragma once
#include "Engine/UI/UIScreen.h"

class Panel;
class Button;
class Text;

class MainMenuScreen : public UIScreen
{
public:
    MainMenuScreen(UISystem* uiSystem);
    virtual ~MainMenuScreen() override;
    
    virtual void Build() override;
    
private:
    Panel* m_backgroundPanel = nullptr;
    Text* m_titleText = nullptr;
    
    Button* m_singleplayerButton = nullptr;
    Button* m_multiplayerButton = nullptr;
    Button* m_settingsButton = nullptr;
    Button* m_quitButton = nullptr;
    
    void BuildBackground();
    void BuildTitle();
    void BuildButtons();
};