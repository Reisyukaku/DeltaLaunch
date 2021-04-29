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
#include <functional>
#include <string>
#include <iomanip>
#include <vector>
#include <filesystem>
#include "GameBase.hpp"
#include "Language.hpp"
#include "../Services/App.hpp"
#include "../Core/Graphics.hpp"
#include "../Core/types.h"

class Game: public GameBase
{
    public:
		Game(u64 tid = 0);
        ~Game();
		
		//Getters/setters
        std::string GetName() { return Name; }
        void SetName(std::string name) { Name = name; }
        std::string GetAuthor() { return Author; }
        void SetAuthor(std::string author) { Author = author; }
        bool IsRunning() override { return App::currentApplication.active && App::currentApplication.tid == TitleID; }
        
        Result Run() override;
        Result DrawOptions() override;
        
    private:
        Result BootGame();
        Language *lang;
        std::string Author;
        UI::MessageBox *msgBox;
        
        /* Functions */
        Result AddToFolder(){
            Result rc = 0;
            Settings::addToFolder = true;
            return rc;
        }
        Result BackupSave(){
            Result rc = 0;
#ifdef __SWITCH__
            rc = MountSave();
            if(R_SUCCEEDED(rc)) {
                std::stringstream stream; stream << std::setfill ('0') << std::setw(16) << std::hex << std::uppercase << TitleID;
                //Create root dir if doesnt exist
                fs::path savePath = fs::path(std::string(SAVE_PATH) + "/");
                if(!fs::exists(savePath))
                    fs::create_directory(savePath);
                //Create title folder if doesnt exist
                fs::path titlePath = savePath / fs::path(stream.str());
                if(!fs::exists(titlePath))
                    fs::create_directory(titlePath);
                
                //Get paths in save
                std::vector<std::string> paths;
                for (const auto & entry : fs::directory_iterator("save:/")) {
                    paths.push_back(entry.path());
                    msgBox->Show(lang->GetString(LANG_ERROR), entry.path());
                }
                UnmountSave();
            }
            else {
                if(rc == 0x7D402)
                    msgBox->Show(lang->GetString(LANG_ERROR), "TitleID or save data doesnt exist!");
                else
                    msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_ERROR) + ": " + std::to_string(rc));
            }
#endif
            
            return rc;
        }
        Result RestoreSave(){
            Result rc = 0;
#ifdef __SWITCH__
            std::stringstream stream; stream << std::setfill ('0') << std::setw(16) << std::hex << std::uppercase << TitleID;
            fs::path titlePath = fs::path(std::string(SAVE_PATH) + "/") / fs::path(stream.str());
            if(fs::exists(titlePath)) {
                rc = MountSave();
                if(R_SUCCEEDED(rc)) {
                    //
                    UnmountSave();
                }
            }
#endif
            
            return rc;
        }
        
        Result MountSave() {
            Result rc = 0;
#ifdef __SWITCH__
            FsFileSystem out;
            
            if(IsRunning()) {
                if(msgBox->Show(lang->GetString(LANG_CLOSING), lang->GetString(LANG_CLOSE_APP), MSGBOX_YESNO) == RESULT_YES) {
                    App::CloseGame();
                }
                else return 0;
            }
            AccountUid uid = Account::GetActiveAccount();

            rc = fsOpen_SaveData(&out, TitleID, uid);
            if(R_SUCCEEDED(rc)) 
                rc = fsdevMountDevice("save", out);
#endif
            return rc;
        }
        
        Result UnmountSave() {
#ifdef __SWITCH__
            fsdevUnmountDevice("save");
#endif
            return 0;
        }
};