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
#include <string>
#include <vector>
#include <fstream>
#include "../Services/Settings.hpp"
#include "../Core/types.h"

enum UiString {
    LANG_NONE = 0,
    LANG_ERROR,
    LANG_WARNING,
    LANG_SUCCESS,
    LANG_UNIMPLEMENTED,
    LANG_PLAYING,
    LANG_START,
    LANG_CLOSING,
    LANG_OPEN,
    LANG_ON,
    LANG_OFF,
    LANG_OK,
    LANG_BACK,
    LANG_YES,
    LANG_NO,
    LANG_ENABLED,
    LANG_DISABLED,
    LANG_ERROR_CODE,
    LANG_FOLDERNAME,
    LANG_SETNAME,
    LANG_FOLDER,
    LANG_NEWS,
    LANG_SETTINGS,
    LANG_REMOVE_SD,
    LANG_THEME_CHANGE,
    LANG_FOLDERFULL,
    LANG_CLOSE_APP,
    LANG_GENCONFIG,
    LANG_INTERNET,
    LANG_DATAMANAGEMENT,
    LANG_USERS,
    LANG_LOOKNFEEL,
    LANG_THEMES,
    LANG_TVSETTINGS,
    LANG_SYSINFO,
    LANG_GENCONFIG_DESC,
    LANG_INTERNET_DESC,
    LANG_DATAMANAGEMENT_DESC,
    LANG_USERS_DESC,
    LANG_LOOKNFEEL_DESC,
    LANG_THEMES_DESC,
    LANG_TVSETTINGS_DESC,
    LANG_SYSINFO_DESC,
    LANG_LOCKSCREEN,
    LANG_AUTORCM,
    LANG_WRITE_NAND,
    LANG_WRITE_COMPLETE,
    LANG_INTERNET_SETTINGS,
    LANG_CHANGE_SPLASH,
    LANG_PREDEFINED_PFP,
    LANG_GAME_SELECT,
    LANG_THEME_SELECT,
    LANG_FIRMWARE,
    LANG_SERIAL,
    LANG_NICKNAME,
    LANG_CONFIG_CONTROL,
    LANG_UPDATE_CONSOLE,
    LANG_UPDATE_CFW,
    LANG_ACTIVE_THEME,
    LANG_OUTLINE,
    LANG_DIFFSIZE,
    LANG_PS4STYLE,
    LANG_DOUBLEROW,
    LANG_ADD_FOLDER,
    LANG_REMOVE_THEME,
    LANG_LANGUAGE,
    LANG_REGION,
    LANG_TIMEZONE,
    LANG_FAIL_SET_PFP,
    LANG_DELETE_TITLE,
    LANG_TV_RES,
    LANG_RGB_RANGE,
    LANG_TV_SND,
    LANG_DATE,
    LANG_JAN,
    LANG_FEB,
    LANG_MAR,
    LANG_APR,
    LANG_MAY,
    LANG_JUN,
    LANG_JUL,
    LANG_AUG,
    LANG_SEP,
    LANG_OCT,
    LANG_NOV,
    LANG_DEC,
	LANG_MAX_FOLDERS
};

class Language
{
    public:        
        static Language* getInstance() {
            if (instance == nullptr)
                instance = new Language();
            return instance;
        };
        
        std::string GetString(UiString str) {
            return uiStrings[str];
        }
        
    private:
        static Language* instance;
        
        Language() {
            std::string line;
            std::ifstream file(std::string(RESOURCES("/Lang/")) + Settings::GetLangStr() + std::string(".txt"));
            while (std::getline(file, line)) //This reads up to \n so remove \r manually if on win.
            {
                uiStrings.push_back(line);
            }
        };
        
        Language(const Language&);
        Language& operator=(const Language&);
        std::vector<std::string> uiStrings;
};