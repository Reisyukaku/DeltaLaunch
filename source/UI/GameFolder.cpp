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

#include "GameFolder.hpp"

GameFolder::GameFolder(u32 folderID, std::string name, Texture *ico) : GameBase(folderID) {
    optBox = UI::ImageOptionBox::getInstance();
    Name = name;
    Icon = ico;
}

GameFolder::~GameFolder() {
	Games.clear();
}

Result GameFolder::DrawOptions() {
    Result rc = 0;
    
    Graphics::RenderTexture(OptTex, OptPos);
    float Y = OptPos.y + 12, X = OptPos.x + 20, space = 25;
    //Title
    Graphics::DrawText(Fonts::FONT_SMALL, X, Y, Name);
    Y+=(space*2);
    //Portrait
    Rect img = {X, Y, 128, 128};
    Graphics::RenderTexture(Icon, img);
    //Info
    Graphics::DrawText(Fonts::FONT_SMALL, X + (img.w + 20), Y+=space, "FolderID: " + std::to_string(TitleID));
    Graphics::DrawText(Fonts::FONT_SMALL, X + (img.w + 20), Y+=space, "Content count: " + std::to_string(Games.size()));
    Y = (OptPos.y + img.h + 62);
    
    //Create menu portion if not created
    if(menu == nullptr) {
        Rect p = {X, Y+=space, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT};
        menu = new Menu(p);
        menu->AddElement(new Button("Delete folder", std::bind(&GameFolder::DeleteFolder, this)));
        //menu->AddElement(new Button("Backup save", std::bind(&GameFolder::BackupSave, this)));
        //menu->AddElement(new Button("Restore save", std::bind(&GameFolder::RestoreSave, this)));
    } 
    menu->Show();
    rc = menu->Update();
    
    return rc;
}

Result GameFolder::Run() {
    Result rc = 0;
    if(Games.size() > 0) {
        /*for(int i = 0; i < (int)Games.size(); i++) 
            optBox->AddOption(Games[i]->GetIcon(), Games[i]->GetName());
        int opt = optBox->Show(Name);
        if(opt >= 0) {
            if(!App::currentApplication.active) {
                rc = Games[opt]->Run();
            }
            else if(App::currentApplication.active && Games[opt]->GetTitleID() != App::currentApplication.tid) {
                App::CloseGame();
                rc = Games[opt]->Run();
            }
            else {
                rc = App::RequestForegroundApp();
            }
        }*/
    }
    else {
        msgBox->Show("Error", "No Games in this folder!");
    }
    return rc;
}