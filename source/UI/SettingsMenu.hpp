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

#pragma once

#include "Game.hpp"
#include "Forms.hpp"
#include "Menus/Menu.hpp"
#include "Menus/Panel.hpp"
#include "../Core/Graphics.hpp"
#include "../Services/Time.hpp"
#include "../Services/Power.hpp"
#include "../Services/Settings.hpp"
#include "../Services/Account.hpp"
#include "../Services/Hid.hpp"

class SettingsMenu: public Menu
{
    public:
        SettingsMenu(Rect pos, Texture *bg);
        ~SettingsMenu();
        
        //Draw override
        void Draw() override;
        
        //Unique functions
        void Initialize() override;
		void Deinit() override;
        
    private:
        
        //Functions
        //General settings ------------------------------------------------
        Result ToggleLockscreen(Panel *pan){
            Result rc = 0;
            bool l = Settings::GetLockScreenFlag();
            Settings::SetLockScreenFlag(!l);
            pan->GetElement(lang->GetString(LANG_LOCKSCREEN))->IncrementOption();
            return rc;
        }
        Result AutoRCM(Panel *pan){
            Result rc = 1;
            if(msgBox->Show(lang->GetString(LANG_WARNING), lang->GetString(LANG_WRITE_NAND), MSGBOX_YESNO) == RESULT_YES) {
                rc = Settings::ToggleAutoRCM();
                msgBox->Show(lang->GetString(LANG_AUTORCM), lang->GetString(LANG_WRITE_COMPLETE));
            }
            return rc;
        }
        Result ChangeSplash(){
            Result rc = 0;
            msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_UNIMPLEMENTED));
            return rc;
        }
        Result AddFolder(){
            Result rc = 0;
            Settings::AddFolder = true;
            return rc;
        }
        //Internet/network settings ---------------------------------------
        Result InternetSettings(){
            Result rc = 0;
            rc = App::LaunchNetConnect(); //0,1,2
            return rc;
        }
        //Manage titles----------------------------------------------------
        Result DeleteTitle() {
            Result rc = 0;
            msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_UNIMPLEMENTED));
            return rc;
        }
        //User customization-----------------------------------------------
        Result CustomPfp(Panel *pan){
            Result rc = 0;
            auto pic = ofd->Show();
            if(pic.string() != "") {
                rc = Account::SetCustomProfileImage(pic.string());
                if(R_FAILED(rc)) msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_FAIL_SET_PFP) + " (" + std::to_string(rc) + ")");
				else {
					pan->GetElement("Profile")->SetImage(Account::GetProfileImage(Account::GetFirstAccount()));
				}
            }
            return rc;
        }
        Result PredefinedPfp(){
            Result rc = 0;
            msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_UNIMPLEMENTED));
            return rc;
        }
        //Look and Feel ---------------------------------------------------
        Result GameSelType(Panel *pan){
            Result rc = 0;
            Settings::gameSelType = ((int)(Settings::gameSelType + 1) > SELECT_DOUBLEROW) ? SELECT_OUTLINE : (SelectType)(Settings::gameSelType + 1);
            pan->GetElement(lang->GetString(LANG_GAME_SELECT))->IncrementOption();
            return rc;
        }
        //Themes ----------------------------------------------------------
        Result ThemeSelect(Panel *pan){
            Result rc = 0;
            ofd->SetFilter(".cfg");
            std::filesystem::path thm = ofd->Show(lang->GetString(LANG_THEME_SELECT), SD("/Themes/"));
            if(!thm.empty()) {
                Settings::currentTheme = thm.filename().string().substr(0, thm.filename().string().find_last_of("."));
                pan->GetElement(lang->GetString(LANG_ACTIVE_THEME))->SetOption(Settings::currentTheme);
                Settings::SaveCurrTheme();
				//Gotta reboot  till we figure out the ram issues
                msgBox->Show(lang->GetString(LANG_WARNING), lang->GetString(LANG_THEME_CHANGE));
                Power::Reboot();
				//TODO make work on switchyboi
#if !defined(__SWITCH__)
				Settings::reloadTheme = true;
#endif
            }
            return rc;
        }
        Result ThemeRemove(Panel *pan){
            Result rc = 0;
            Settings::currentTheme = "";
            pan->GetElement(lang->GetString(LANG_REMOVE_THEME))->SetOption(lang->GetString(LANG_NONE));
            Settings::SaveCurrTheme();
            //msgBox->Show(lang->GetString(LANG_WARNING), lang->GetString(LANG_THEME_CHANGE));
            //Power::Reboot();
			Settings::reloadTheme = true;
            return rc;
        }
        //TV settings -----------------------------------------------------
        Result TvResolution(Panel *pan) {
            Result rc = 0;
            msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_UNIMPLEMENTED));
            return rc;
        }
        Result RgbRange(Panel *pan) {
            Result rc = 0;
            msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_UNIMPLEMENTED));
            return rc;
        }
        Result TvSound(Panel *pan) {
            Result rc = 0;
            msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_UNIMPLEMENTED));
            return rc;
        }
        //System settings -------------------------------------------------
        Result SetLang(Panel *pan){
            Result rc = 0;
            //TODO
            pan->GetElement(lang->GetString(LANG_LANGUAGE))->IncrementOption();
            return rc;
        }
        Result SetRegion(Panel *pan){
            Result rc = 0;
            //TODO
            pan->GetElement(lang->GetString(LANG_REGION))->SetOption(Settings::GetAvailRegions()[Settings::GetRegionIndex()]);
            return rc;
        }
        Result ConfigControl(){
            Result rc = 0;
            msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_UNIMPLEMENTED));
            rc = App::LaunchController();
            return rc;
        }
        Result TimeZone(Panel *pan){
            Result rc = 0;
            pan->GetElement(lang->GetString(LANG_TIMEZONE))->SetOption(Time::GetTimezoneName());
            return rc;
        }
        Result SetDate(Panel *pan){
            Result rc = 0;
            pan->GetElement(lang->GetString(LANG_DATE))->SetOption(Time::GetDate());
            return rc;
        }
        Result SetNick(Panel *pan){
            Result rc = 0;
            char out[0xC00] = {0};
            App::LaunchSwkbd(out, lang->GetString(LANG_NICKNAME), "", lang->GetString(LANG_SETNAME), Settings::GetDeviceNickname());
            std::string name = std::string(out);
            Settings::SetDeviceNickname(name);
            pan->GetElement(lang->GetString(LANG_NICKNAME))->SetOption(name);
            return rc;
        }
        Result UpdateCFW(){
            Result rc = 0;
            //std::string res = net->Request("http://conntest.nintendowifi.net/", HTTP_GET);
            //msgBox->Show(lang->GetString(LANG_ERROR), res);
            return rc;
        }
        Result UpdateConsole(){
            Result rc = 0;
            msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_UNIMPLEMENTED));
            return rc;
        }
        //-----------------------------------------------------------------
};