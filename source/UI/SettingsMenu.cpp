/*
* ΔLaunch
* Copyright (c) 2018-2019, Reisyukaku
* 
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
* 
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "SettingsMenu.hpp"

SettingsMenu::SettingsMenu(Rect pos, Texture *bg) : Menu(pos, bg, LANG_SETTINGS){
	//Panel location
    panX = 450;
    panY = 100;
}

SettingsMenu::~SettingsMenu() {
	//
}

void SettingsMenu::Initialize() {    
    //General settings
    AddElement(new Button(lang->GetString(LANG_GENCONFIG)));
    Panel *gen = new Panel(panX, panY);
    gen->AddString(std::string(lang->GetString(LANG_GENCONFIG_DESC)));
    gen->AddElement(new Option(lang->GetString(LANG_LOCKSCREEN), {lang->GetString(LANG_OFF), lang->GetString(LANG_ON)}, Settings::GetLockScreenFlag(), std::bind(&SettingsMenu::ToggleLockscreen, this, gen)));
    gen->AddElement(new Option(lang->GetString(LANG_AUTORCM), {lang->GetString(LANG_DISABLED), lang->GetString(LANG_ENABLED)}, Settings::HasAutoRCM(), std::bind(&SettingsMenu::AutoRCM, this, gen)));
    gen->AddElement(new Button(lang->GetString(LANG_CHANGE_SPLASH), std::bind(&SettingsMenu::ChangeSplash, this)));
    gen->AddElement(new Button(lang->GetString(LANG_ADD_FOLDER), std::bind(&SettingsMenu::AddFolder, this)));
    gen->AddElement(new Button(lang->GetString(LANG_CONFIG_CONTROL), std::bind(&SettingsMenu::ConfigControl, this)));
    Panels.push_back(gen);
    
    //Internet/network settings
    AddElement(new Button(lang->GetString(LANG_INTERNET)));
    Panel *internet = new Panel(panX, panY);
    internet->AddString(std::string(lang->GetString(LANG_INTERNET_DESC)));
    internet->AddElement(new Button(lang->GetString(LANG_INTERNET_SETTINGS), std::bind(&SettingsMenu::InternetSettings, this)));
    Panels.push_back(internet);
    
    //Manage titles
    AddElement(new Button(lang->GetString(LANG_DATAMANAGEMENT)));
    Panel *dataMan = new Panel(panX, panY);
    dataMan->AddString(std::string(lang->GetString(LANG_DATAMANAGEMENT_DESC)));
    dataMan->AddElement(new Button(lang->GetString(LANG_DELETE_TITLE), std::bind(&SettingsMenu::DeleteTitle, this)));
    Panels.push_back(dataMan);
    
    //User customization
    AddElement(new Button(lang->GetString(LANG_USERS)));
    Panel *user = new Panel(panX, panY);
    user->AddString(std::string(lang->GetString(LANG_USERS_DESC)));
    user->AddElement(new Image("Profile", 256, 256, Account::GetProfileImage(Account::GetFirstAccount()), std::bind(&SettingsMenu::CustomPfp, this, user)));
    user->AddElement(new Button(lang->GetString(LANG_PREDEFINED_PFP), std::bind(&SettingsMenu::PredefinedPfp, this)));
    Panels.push_back(user);
    
    //Look and feel
    AddElement(new Button(lang->GetString(LANG_LOOKNFEEL)));
    Panel *look = new Panel(panX, panY);
    look->AddString(std::string(lang->GetString(LANG_LOOKNFEEL_DESC)));
    look->AddElement(new Option(lang->GetString(LANG_GAME_SELECT), {lang->GetString(LANG_OUTLINE), lang->GetString(LANG_DIFFSIZE), lang->GetString(LANG_PS4STYLE), lang->GetString(LANG_DOUBLEROW)}, Settings::gameSelType, std::bind(&SettingsMenu::GameSelType, this, look)));
    Panels.push_back(look);
    
    //Themes
    AddElement(new Button(lang->GetString(LANG_THEMES)));
    Panel *theme = new Panel(panX, panY);
    theme->AddString(std::string(lang->GetString(LANG_THEMES_DESC)));
    theme->AddElement(new Option(lang->GetString(LANG_ACTIVE_THEME), Settings::currentTheme, 0, std::bind(&SettingsMenu::ThemeSelect, this, theme)));
    theme->AddElement(new Button(lang->GetString(LANG_REMOVE_THEME), std::bind(&SettingsMenu::ThemeRemove, this, theme)));
    Panels.push_back(theme);
    
    //TV settings
    AddElement(new Button(lang->GetString(LANG_TVSETTINGS)));
    Panel *tvSet = new Panel(panX, panY);
    tvSet->AddString(std::string(lang->GetString(LANG_TVSETTINGS_DESC)));
    tvSet->AddElement(new Option(lang->GetString(LANG_TV_RES), "", 0, std::bind(&SettingsMenu::TvResolution, this, tvSet)));
    tvSet->AddElement(new Option(lang->GetString(LANG_RGB_RANGE), "", 0, std::bind(&SettingsMenu::RgbRange, this, tvSet)));
    tvSet->AddElement(new Option(lang->GetString(LANG_TV_SND), "", 0, std::bind(&SettingsMenu::TvSound, this, tvSet)));
    Panels.push_back(tvSet);
    
    //System information and updates
    AddElement(new Button(lang->GetString(LANG_SYSINFO)));
    Panel *sysinfo = new Panel(panX, panY);
    sysinfo->AddString(std::string(lang->GetString(LANG_SYSINFO_DESC)));
    sysinfo->AddString(std::string(lang->GetString(LANG_FIRMWARE) + ": " + Settings::GetFirmwareVersion()));
    sysinfo->AddString(std::string(lang->GetString(LANG_SERIAL) + ": " + Settings::GetSerialNumber()));
    sysinfo->AddElement(new Option(lang->GetString(LANG_LANGUAGE), Settings::GetAvailLangs(), Settings::GetLangIndex(), std::bind(&SettingsMenu::SetLang, this, sysinfo)));
    sysinfo->AddElement(new Option(lang->GetString(LANG_REGION), Settings::GetAvailRegions(), (u32)Settings::GetRegionIndex(), std::bind(&SettingsMenu::SetRegion, this, sysinfo)));
    sysinfo->AddElement(new Option(lang->GetString(LANG_TIMEZONE), Time::GetTimezoneName(), 0, std::bind(&SettingsMenu::TimeZone, this, sysinfo)));
    sysinfo->AddElement(new Option(lang->GetString(LANG_DATE), Time::GetDate(), 0, std::bind(&SettingsMenu::SetDate, this, sysinfo)));
    sysinfo->AddElement(new Option(lang->GetString(LANG_NICKNAME), Settings::GetDeviceNickname(), 0, std::bind(&SettingsMenu::SetNick, this, sysinfo)));
    sysinfo->AddElement(new Button(lang->GetString(LANG_UPDATE_CFW), std::bind(&SettingsMenu::UpdateCFW, this)));
    sysinfo->AddElement(new Button(lang->GetString(LANG_UPDATE_CONSOLE), std::bind(&SettingsMenu::UpdateConsole, this)));
    Panels.push_back(sysinfo);
}

void SettingsMenu::Deinit() {
	for (auto &pan : Panels) delete pan;
	for (auto &mel : MenuElements) delete mel;
	Panels.clear();
	MenuElements.clear();
}

/*
*   Draw/Set Graphics
*/
void SettingsMenu::Draw() {
    int ind = 0;
    Rect p = Pos;
    p.y += MENU_BUTTON_START_Y;
    p.x += MENU_BUTTON_INDENT;
    p.w = MENU_BUTTON_WIDTH;
    p.h = MENU_BUTTON_HEIGHT;
    for(auto button: MenuElements) {
        button->Draw((ind != menuOpt) ? ELEM_Unselected : (!currLayer ? (ElementType)(ELEM_Selected|ELEM_DrawOutline) : (ELEM_Selected)), p);
        if(Hid::IsTouched(p)) {
            menuOpt = ind;
            currLayer = 0;
        }
        p.y += (p.h + elemSpace);
        ind++;
    }
    Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + Pos.w - 260, Pos.y + Pos.h - 50, std::string(FON_B) + " " + lang->GetString(LANG_BACK));
    Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + Pos.w - 130, Pos.y + Pos.h - 50, std::string(FON_A) + " " + lang->GetString(LANG_OK));
}