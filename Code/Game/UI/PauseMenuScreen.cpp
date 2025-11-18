// Game/UI/PauseMenuScreen.cpp
#include "PauseMenuScreen.h"
#include "Engine/UI/Canvas.hpp"
#include "Engine/UI/Panel.h"
#include "Engine/UI/Button.h"
#include "Engine/UI/Text.h"
#include "Engine/Core/EngineCommon.hpp"

PauseMenuScreen::PauseMenuScreen(UISystem* uiSystem)
    : UIScreen(uiSystem, UIScreenType::PAUSE_MENU, true)
{
}

PauseMenuScreen::~PauseMenuScreen()
{
}

void PauseMenuScreen::Build()
{
    if (!m_canvas || !m_camera)
    {
        return;
    }
    
    m_camera->SetOrthographicView(Vec2(0, 0), Vec2(1600, 900));
    
    BuildBackground();
    BuildTitle();
    BuildButtons();
}

void PauseMenuScreen::BuildBackground()
{
    // 全屏半透明遮罩
    AABB2 screenBounds(0, 0, 1600, 900);
    m_backgroundDimmer = new Panel(
        m_canvas,
        screenBounds,
        Rgba8(0, 0, 0, 200),
        nullptr,
        false,
        Rgba8::BLACK
    );
    m_elements.push_back(m_backgroundDimmer);
    
    // 菜单面板
    AABB2 menuBounds(550, 200, 1050, 700);
    m_menuPanel = new Panel(
        m_canvas,
        menuBounds,
        Rgba8(50, 50, 50),
        nullptr,
        true,
        Rgba8(120, 120, 120)
    );
    m_elements.push_back(m_menuPanel);
}

void PauseMenuScreen::BuildTitle()
{
    TextSetting titleSetting;
    titleSetting.m_text = "Game Paused";
    titleSetting.m_color = Rgba8(230, 230, 230);
    titleSetting.m_height = 48.0f;
    
    Vec2 titlePos(800, 650);  // 居中显示
    m_titleText = new Text(m_canvas, titlePos, titleSetting);
    m_elements.push_back(m_titleText);
}

void PauseMenuScreen::BuildButtons()
{
    float buttonWidth = 350.0f;
    float buttonHeight = 55.0f;
    float spacing = 20.0f;
    float startY = 560.0f;
    float centerX = 800.0f;
    
    // Resume 按钮
    AABB2 resumeBounds(
        centerX - buttonWidth * 0.5f, startY,
        centerX + buttonWidth * 0.5f, startY + buttonHeight
    );
    m_resumeButton = new Button(
        nullptr,
        resumeBounds,
        Rgba8(100, 200, 100),  // 绿色 hover
        Rgba8::WHITE,
        "ResumeGame",
        "Back to Game",
        Vec2(0.5f, 0.5f)
    );
    m_canvas->AddElementToCanvas(m_resumeButton);
    m_elements.push_back(m_resumeButton);
    
    // Settings 按钮
    startY -= (buttonHeight + spacing);
    AABB2 settingsBounds(
        centerX - buttonWidth * 0.5f, startY,
        centerX + buttonWidth * 0.5f, startY + buttonHeight
    );
    m_settingsButton = new Button(
        nullptr,
        settingsBounds,
        Rgba8(100, 150, 200),  // 蓝色 hover
        Rgba8::WHITE,
        "OpenSettings",
        "Settings",
        Vec2(0.5f, 0.5f)
    );
    m_canvas->AddElementToCanvas(m_settingsButton);
    m_elements.push_back(m_settingsButton);
    
    // Save Game 按钮
    startY -= (buttonHeight + spacing);
    AABB2 saveBounds(
        centerX - buttonWidth * 0.5f, startY,
        centerX + buttonWidth * 0.5f, startY + buttonHeight
    );
    m_saveButton = new Button(
        nullptr,
        saveBounds,
        Rgba8(200, 180, 100),  // 黄色 hover
        Rgba8::WHITE,
        "SaveGame",
        "Save Game",
        Vec2(0.5f, 0.5f)
    );
    m_canvas->AddElementToCanvas(m_saveButton);
    m_elements.push_back(m_saveButton);
    
    // Main Menu 按钮
    startY -= (buttonHeight + spacing);
    AABB2 mainMenuBounds(
        centerX - buttonWidth * 0.5f, startY,
        centerX + buttonWidth * 0.5f, startY + buttonHeight
    );
    m_mainMenuButton = new Button(
        nullptr,
        mainMenuBounds,
        Rgba8(200, 120, 100),  // 橙红色 hover
        Rgba8::WHITE,
        "BackToMainMenu",
        "Save and Quit to Title",
        Vec2(0.5f, 0.5f)
    );
    m_canvas->AddElementToCanvas(m_mainMenuButton);
    m_elements.push_back(m_mainMenuButton);
    
    // Quit Game 按钮
    startY -= (buttonHeight + spacing);
    AABB2 quitBounds(
        centerX - buttonWidth * 0.5f, startY,
        centerX + buttonWidth * 0.5f, startY + buttonHeight
    );
    m_quitButton = new Button(
        nullptr,
        quitBounds,
        Rgba8(200, 80, 80),  // 红色 hover
        Rgba8::WHITE,
        "QuitGame",
        "Quit to Desktop",
        Vec2(0.5f, 0.5f)
    );
    m_canvas->AddElementToCanvas(m_quitButton);
    m_elements.push_back(m_quitButton);
}

void PauseMenuScreen::OnEnter()
{
    UIScreen::OnEnter();
    // 暂停游戏音乐
    // g_theAudio->PauseMusic();
}

void PauseMenuScreen::OnExit()
{
    UIScreen::OnExit();
    // 恢复游戏音乐
    // g_theAudio->ResumeMusic();
}