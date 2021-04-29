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
#include <tuple>
#include "Button.hpp"
#include "Option.hpp"
#include "Image.hpp"
#include "UIElement.hpp"
#include "../../Core/Audio.hpp"
#include "../../Core/Graphics.hpp"
#include "../../Services/Hid.hpp"
#include "../../Core/types.h"

#define PANEL_BUTTON_WIDTH 450
#define PANEL_BUTTON_HEIGHT 40
#define PANEL_ELEMENT_SPACE 15

class Panel
{
    public:
        Panel(float x = 0, float y = 0) {
            Pos = {x, y, PANEL_BUTTON_WIDTH, PANEL_BUTTON_HEIGHT};
            optSelect = 0;
        }
        
        ~Panel() {
			for (auto &el : Elements) delete el;
            Elements.clear();
            Strings.clear();
        }
        
        void AddString(std::string str) { 
            Strings.push_back(str);
        }
        
        void AddElement(UIElement *op) {
            Elements.push_back(op);
        }
        
        void SetImage(u32 ind, Texture *tex) {
            u32 i = 0;
            for(auto elem: Elements) {
                if(elem->Properties == ELEM_Image) {
                    if(ind == i) {
                        elem->SetImage(tex);
                    }
                    i++;
                }
            }
        }
        
        u16 Update(u16 selected = 0) {
            unsigned ind = 0;
            posY = Pos.y;
            u16 select = selected;
            bool act = false;
    
            //Draw strings
            for(auto str: Strings) {
                Graphics::DrawText(Fonts::FONT_SMALL, Pos.x, posY, Strings[ind]);
                posY += (PANEL_ELEMENT_SPACE * 2);
                ind++;
            }
            posY += 10;
            
            //Draw UIElements
            ind = 0;
            for(auto elem: Elements) {
                Rect pos = Pos;
                pos.y = posY;
                
                //Draw elems
                switch(elem->Properties) {
                    case ELEM_Image: {
                        elem->Draw(((ind == optSelect) && (bool)selected) ? ELEM_Selected : ELEM_Unselected, pos);
                        posY += (static_cast<Image*>(elem)->Height + PANEL_ELEMENT_SPACE); 
                        break;
                    }
                    default: {
                        elem->Draw(((ind == optSelect) && (bool)selected) ? (ElementType)(ELEM_Selected|ELEM_DrawOutline) : ELEM_Unselected, pos);
                        posY += (Pos.h + PANEL_ELEMENT_SPACE);
                        break;
                    }
                }
                
                //Touch defect
                if(Hid::IsTouched(pos)) {
                    if(optSelect == ind) {
                        act = true;
                    }
                    else {
                        optSelect = ind;
                        select = 1;
                    }
                }
                
                ind++;
            }
            if(act) Activate();
            
            return select;
        }
        
        void Activate() {
            Audio::Play(SFX_Activate);
            if(Elements[optSelect]->HasFunc())
                Elements[optSelect]->Run();
        }
        
        void IncrementSelect() {
            Audio::Play(SFX_Navigate);
            if(optSelect < Elements.size()-1)
                optSelect++;
            else
                optSelect = Elements.size()-1;
        }
        
        void DecrementSelect() {
            Audio::Play(SFX_Navigate);
            if(optSelect > 0)
                optSelect--;
            else
                optSelect = 0;
        }
        
        u32 ElementCnt() { 
            return Elements.size();
        }
        
        UIElement *GetElement(std::string name) {
            for(auto el :Elements) {
                if(el->Name == name) return el;
            }
            return nullptr;
        }
        
    private:
        Rect Pos;
        u32 optSelect;
        u32 posX, posY;
        std::vector<UIElement*> Elements;
        std::vector<std::string> Strings;
};