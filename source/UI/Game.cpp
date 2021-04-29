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

#include "Game.hpp"

Game::Game(u64 tid) : GameBase(tid) {
    //
}

Game::~Game() {
	if(Icon) Graphics::FreeTexture(Icon);
}

Result Game::BootGame() {
    Result rc = 0;
    if(R_FAILED(rc)) {
        msgBox->Show("Game", "Error: nsCheckApplicationLaunchVersion");
        return rc;
    }
    AccountUid userid = Account::GetActiveAccount();
    if(!accountUidIsValid(&userid)) return 0;
    rc = App::LaunchGame(TitleID, userid);
    if(R_FAILED(rc)) {
        msgBox->Show("Game", "Error launching game!");
        return rc;
    }
    return rc;
}

Result Game::DrawOptions() {
    Result rc = 0;
    
    Graphics::RenderTexture(OptTex, OptPos);
    s32 Y = OptPos.y + 12, X = OptPos.x + 20, space = 25;
    //Title
    Graphics::DrawText(Fonts::FONT_SMALL, X, Y, Name);
    Y+=(space*2);
    //Portrait
    Rect img = {X, Y, 128, 128};
    Graphics::RenderTexture(Icon, img);
    //Info
    Graphics::DrawText(Fonts::FONT_SMALL, X + (img.w + 20), Y+=space, "Version: " + Version);
    Graphics::DrawText(Fonts::FONT_SMALL, X + (img.w + 20), Y+=space, "Author: " + Author);
    Graphics::DrawText(Fonts::FONT_SMALL, X + (img.w + 20), Y+=space, "TitleID: " + Utils::NumToHexStr(TitleID));
    Graphics::DrawText(Fonts::FONT_SMALL, X + (img.w + 20), Y+=space, "Type: " + std::to_string(Type));
    Y = (OptPos.y + img.h + 62);
    
    //Create menu portion if not created
   if(menu == nullptr) {
        Rect p = {X, Y+=space, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT};
        menu = new Menu(p);
        menu->AddElement(new Button("Add to folder", std::bind(&Game::AddToFolder, this)));
        menu->AddElement(new Button("Backup save", std::bind(&Game::BackupSave, this)));
        menu->AddElement(new Button("Restore save", std::bind(&Game::RestoreSave, this)));
    } 
    menu->Show();
    rc = menu->Update();
    
    return rc;
}

Result Game::Run() {
    return BootGame();
}