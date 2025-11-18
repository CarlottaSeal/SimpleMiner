#pragma once
#include "Engine/UI/UIScreen.h"

class Panel;
class Button;
class Text;
class Slider;
class Checkbox;

class SettingsScreen : public UIScreen
{
public:
    SettingsScreen(UISystem* uiSystem);
    virtual ~SettingsScreen() override;
    
    virtual void Build() override;
    
private:
    Panel* m_backgroundDimmer = nullptr;
    Panel* m_settingsPanel = nullptr;
    Text* m_titleText = nullptr;
    
    // 音量设置
    Text* m_masterVolumeLabel = nullptr;
    Slider* m_masterVolumeSlider = nullptr;
    Text* m_musicVolumeLabel = nullptr;
    Slider* m_musicVolumeSlider = nullptr;
    Text* m_sfxVolumeLabel = nullptr;
    Slider* m_sfxVolumeSlider = nullptr;
    
    // 图形设置
    Text* m_fovLabel = nullptr;
    Slider* m_fovSlider = nullptr;
    Checkbox* m_fullscreenCheckbox = nullptr;
    Checkbox* m_vsyncCheckbox = nullptr;
    
    // 按钮
    Button* m_doneButton = nullptr;
    
    void BuildBackground();
    void BuildTitle();
    void BuildAudioSettings();
    void BuildGraphicsSettings();
    void BuildButtons();
    
    void OnDoneClicked();
    void OnMasterVolumeChanged();
    void OnMusicVolumeChanged();
    void OnSFXVolumeChanged();
};