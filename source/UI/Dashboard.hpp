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
#include <vector>
#include <functional>
#include <algorithm>
#include <tuple>
#include <string>
#include "Forms.hpp"
#include "Game.hpp"
#include "GameFolder.hpp"
#include "SettingsMenu.hpp"
#include "NewsMenu.hpp"
#include "../Core/Graphics.hpp"
#include "../Core/Audio.hpp"
#include "../Core/Utils/INI.hpp"
#include "../Core/Utils.hpp"
#include "../Services/Hid.hpp"
#include "../Services/Rnx.hpp"
#include "../Services/Settings.hpp"
#include "../Services/Time.hpp"
#include "../Services/Power.hpp"
#include "../Services/App.hpp"
#include "../Services/Hid.hpp"
#include "../Core/types.h"

#define BACKGROUND_LAYERS 99
#define MAX_GAMES_PER_FOLDER 16
#define MAX_FOLDERS 10

class Dashboard
{
    public:
        Dashboard(u32 width, u32 height);
        ~Dashboard();
        
        //Initialize
        void Initialize();
        void PopulateFolders();
        void SetGames();
        
        //Draw/set
        void DrawLockScreen();
        void SetLockScreen(std::string image, std::string lock);
        void DrawButtons();
		void DrawGames();
        void DrawCanvas();
        void DrawDebugText();
        void CheckEvents();
        
        //Folder mech
        void AddFolder();
        void DeleteFolder(u64 folder);
        void MoveSelectedGameToFolder(u64 folderID);
        
        //Interactions
        void SelectGame(GameBase *gbase, s32 ind);
        void OffsetGameIcons(u32 deltaX);
		void IncrementDashSel();
		void DecrementDashSel();
        void ActivateDash();
        void CloseApp();
        void ChangeLayer(int l);
        void UpdateDash();
        void IncrementLayer();
		void DecrementLayer();
		
		//Menus
		Result OpenSettings();
        Result OpenNews();
        void UpdateMenus();
        
        bool IsMenuOpen() { return (settings && settings->IsOpen()) || (news && news->IsOpen()); }
        bool ReturnToLock() { return App::wakingup; }

        u32 MaxColumns;
		u8 gameRows;
		SettingsMenu *settings;
		UI::ImageOptionBox *optBox;
		UI::OpenFileDialog *ofd;
        Language *lang;
        NewsMenu *news;
		UI::MessageBox *msgBox;
        
    private:
        u32 lastPosX;
        u32 gamePicSize, gameSpacing;
        SelectType lastMode;
        bool debugInfo;
        bool GcState;
        bool draggingGameArea;
        bool gameOptionsActive;
		Result lastErr;
        Rect BatPos, ClkPos, WindowPos, GameIconArea, GameSelect, gameOptPos;
        Texture *Wallpaper;
        Texture *LockScreen;
        Texture *LockIcon;
        Texture *Battery;
        Texture *folderIcon;
		Texture *menuTex;
		Texture *dummyGame;
		Texture *formTex;
		Texture *msgTex;
		Texture *newsTex; //temp?
        std::vector<AppletIcon*> AppletIcons;
		std::vector<GameBase*> GameEntries;
};