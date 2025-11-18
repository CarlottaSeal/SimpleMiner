#pragma once
#include "Engine/UI/UIScreen.h"

class Panel;
class Button;
class Text;

class PauseMenuScreen : public UIScreen
{
public:
    PauseMenuScreen(UISystem* uiSystem);
    virtual ~PauseMenuScreen() override;
    
    virtual void Build() override;
    virtual void OnEnter() override;
    virtual void OnExit() override;
    
private:
    Panel* m_backgroundDimmer = nullptr;
    Panel* m_menuPanel = nullptr;
    Text* m_titleText = nullptr;
    
    Button* m_resumeButton = nullptr;
    Button* m_settingsButton = nullptr;
    Button* m_saveButton = nullptr;
    Button* m_mainMenuButton = nullptr;
    Button* m_quitButton = nullptr;
    
    void BuildBackground();
    void BuildTitle();
    void BuildButtons();
    
    // 按钮回调
    void OnResumeClicked();
    void OnSettingsClicked();
    void OnSaveClicked();
    void OnMainMenuClicked();
    void OnQuitClicked();
};