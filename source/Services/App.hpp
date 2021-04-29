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
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include "Power.hpp"
#include "Settings.hpp"
#include "../Core/Graphics.hpp"
#include "../UI/Forms.hpp"
#include "../Core/types.h"

enum Command {
    CMD_Home = 12,
    CMD_Sleep = 16,
    CMD_Wakeup = 20,
    CMD_Shutdown = 22,
    CMD_Reboot = 24,
    CMD_RemovedSD = 72,
    
};

class App
{
    public:
		static Result LaunchGame(u64 tid, AccountUid userID);
        static void CloseGame();
        static Result GetAppRecords(std::vector<NsApplicationRecord> &recs);
		static NsApplicationControlData GetGameControlData(u64 tid, NsApplicationControlSource flag);
        static bool IsGamecardInserted();
		static Result LaunchAlbum(u8 arg, bool startupSound);
        static AccountUid LaunchPSelect();
		static Result LaunchShop();
        static Result LaunchWebsite(std::string url);
        static Result LaunchNetConnect(u32 arg = 0);
        static Result LaunchController();
        static Result LaunchSwkbd(char out[0xc00], std::string title, std::string placehold, std::string oktxt, std::string initTxt);
		static Result ShowError(std::string text1, std::string text2, Result rc);
		static Result LaunchHbl();
        static Result RequestForegroundApp();
		static void RequestForegroundDelta();
        static Result CommandHandler(u32 cmd);
		
		static AppletApplication currentApplication;
        static AppletHolder currentApplet;
        static s32 gameSelectInd;
        static s32 appletSelectInd;
        static u8 dashLayer;
        
        static u32 lastAeCmd;
		static u32 lastSamsCmd;
        static bool sdRemoved;
        static bool wakingup;
        static bool refreshTheme;
};