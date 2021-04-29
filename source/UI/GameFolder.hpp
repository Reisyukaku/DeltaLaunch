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
#include <iomanip>
#include <vector>
#include <string>
#include "GameBase.hpp"

class GameFolder: public GameBase
{
    public:
		GameFolder(u32 folderID, std::string name, Texture *ico);
        ~GameFolder();
        
        void SetName(std::string name) { Name = name; }
        void AddGame(u64 tid, Texture *ico, std::string name) { Games.push_back(std::tuple<u64, Texture*, std::string>(tid, ico, name)); }
        bool IsFolder() override{ return true; }
        
        Result Run() override;
        Result DrawOptions() override;
        
        /* Functions */
        Result DeleteFolder() {
            Result rc = 0;
            Settings::deleteFolder = (s32)TitleID;
            return rc;
        }
        std::vector<std::tuple<u64, Texture*, std::string>> Games;
        
    private:
        UI::ImageOptionBox *optBox;
};