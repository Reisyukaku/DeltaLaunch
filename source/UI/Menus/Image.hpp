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

#include "UIElement.hpp"
#include "../../Core/types.h"

class Image: public UIElement
{
    public:
        Image(std::string name, u32 w, u32 h, Texture *tex, std::function<Result()> callback = nullptr) : UIElement() {
            Tex = tex;
            Properties = ELEM_Image;
            Callback = callback;
            Width = w;
            Height = h;
            Name = name;
        }
        
        ~Image() {
            if(Tex) Graphics::FreeTexture(Tex);
        };
        
        Result Run() override {
            return Callback();
        }
        
        void Draw(ElementType type, Rect pos) override {
            Rect p = pos;
            p.w = Width;
            p.h = Height;
            Graphics::DrawImageElem(Tex, p, type);
        }
        
        void SetImage(Texture *tex) override {
            if(Tex) Graphics::FreeTexture(Tex);
            Tex = tex;
        }
        
        u32 Width, Height;
    private:
        Texture *Tex;
};