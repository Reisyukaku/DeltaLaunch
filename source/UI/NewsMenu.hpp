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
#include "../Services/Hid.hpp"

class NewsMenu: public Menu
{
    public:
        NewsMenu(Rect pos, Texture *bg, Texture *article);
        ~NewsMenu();

		void Initialize() override;
		void Deinit() override;
        
        //Draw override
        void Draw() override;
        
        
        //Function
        std::function<Result()> OpenNews = [&]()->Result{
            Result rc = 0;
            msgBox->Show("Test", "hello ", MSGBOX_OK);
            return rc;
        };

		Texture *Article;
};