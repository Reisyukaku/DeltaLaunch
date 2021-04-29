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
#include "UIElement.hpp"
#include "../../Core/Graphics.hpp"
#include "../../Core/types.h"

class AppletIcon: public UIElement
{
    public:
        AppletIcon(std::string sprite, std::string spriteSel, u32 x, u32 y, std::function<Result()> callback = nullptr) : UIElement() {
            Callback = callback;
            Properties = ELEM_Button;
            Pos.x = x;
            Pos.y = y;
			if (sprite != "") {
				Sprite = Graphics::CreateTexFromString(sprite);
				Pos.w = Sprite->w;
				Pos.h = Sprite->h;
			}
            if(spriteSel != "") 
                SpriteSelect = Graphics::CreateTexFromString(spriteSel);
        }
        
        ~AppletIcon() {
            if(Sprite) Graphics::FreeTexture(Sprite);
            if(SpriteSelect) Graphics::FreeTexture(SpriteSelect);
        }
        
        void Draw(ElementType butType, Rect pos = {0,0,0,0}) override{
            if(butType & ELEM_Selected)
                Graphics::RenderTexture(SpriteSelect, Pos);
			else
				Graphics::RenderTexture(Sprite, Pos);
        }
        
        Result Run() override {
            Result res = 0;
            if(Callback != nullptr) res = Callback();
            return res;
        }
        
        bool operator <(const AppletIcon & but) const {
            return this->Pos.x < but.Pos.x;
        }
        
        //vars
        Texture *Sprite;
		Texture *SpriteSelect;
        Rect Pos;
};