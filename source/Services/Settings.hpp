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
#include <cstdio>
#include <cstring>
#include <vector>
#include <filesystem>
#include "../Core/Utils/INI.hpp"
#include "../Core/types.h"

enum SelectType{
	SELECT_OUTLINE,
	SELECT_SIZEDIFF,
    SELECT_PSSTYLE,
    SELECT_DOUBLEROW
};

enum GameAreaType{
    GAMEAREA_FULL,
    GAMEAREA_MINI
};

class Settings
{
    public:
        static SelectType gameSelType;
        static GameAreaType gameAreaType;
        static std::string GetFirmwareVersion();
        static std::string GetDeviceNickname();
        static u64 GetSystemLangCode();
        static void SetDeviceNickname(std::string nick);
		static std::string GetSerialNumber();
        static bool GetLockScreenFlag();
        static void SetLockScreenFlag(bool flag);
        static size_t GetMemAvail();
        static size_t GetMemUsed();
        static bool GetLockIconEnabled() { return LockIconEnabled; }
        static void SetLockIconEnabled(bool b) { LockIconEnabled = b; }
        static std::string GetThemePath();
        static std::string GetLangStr() { return Langs[GetLangIndex()]; }
        static std::vector<std::string> GetAvailLangs() { return Langs; }
        static SetRegion GetRegionIndex();
        static std::vector<std::string> GetAvailRegions() { return Regions; }
        static SetLanguage GetLangIndex();
        static bool HasAutoRCM();
        static Result ToggleAutoRCM();
        static void SaveCurrTheme();
        static void LoadSavedTheme();
        
        static std::string currentTheme;
        static bool AddFolder;
        static s32 deleteFolder;
        static bool addToFolder;
		static bool reloadTheme;
    private:
        static bool LockIconEnabled;
        static std::vector<std::string> Langs;
        static std::vector<std::string> Regions;
        
};