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
#include "../Forms.hpp"
#include "Panel.hpp"
#include "../Language.hpp"
#include "../../Core/Graphics.hpp"
#include "../../Core/Audio.hpp"
#include "../../Services/Network.hpp"
#include "../../Core/types.h"

#define MENU_BUTTON_INDENT 60
#define MENU_BUTTON_START_Y 100
#define MENU_ELEMENT_SPACE 15
#define MENU_BUTTON_WIDTH 280
#define MENU_BUTTON_HEIGHT 50

class Menu
{
    public:
		Menu(Rect pos, Texture *bg = nullptr, UiString title = LANG_NONE) {
			msgBox = UI::MessageBox::getInstance();
			ofd = UI::OpenFileDialog::getInstance();
			net = Network::getInstance();
			lang = Language::getInstance();
			elemSpace = MENU_ELEMENT_SPACE;
			Title = title == LANG_NONE ? "" : lang->GetString(title);
			Pos = pos;
			Visible = false;
			currLayer = 0;
			menuOpt = 0;
			Sprite = nullptr;
			if (bg) Sprite = bg;
		}
		virtual ~Menu() {
            //
        }
		
		//Exposed Getters/Setters
        bool IsOpen() { return Visible; }
		u32 GetSelection() {return menuOpt;}
		void SetSelection(u16 sel) {menuOpt = sel;}
		std::string GetTitle() { return Title; }
		void AddElement(UIElement *elem) { MenuElements.push_back(elem); }
		void Show() { Visible = true; }
		void Hide() { Visible = false; }
		
		virtual void Initialize() {}
		virtual void Deinit() {}
		
		//Super class overrides
        virtual void Draw() {
            u32 ind = 0;
            Rect p = Pos;
            for(auto &button: MenuElements) {
                button->Draw(((ind != menuOpt)) ? ELEM_Unselected : (!currLayer ? (ElementType)(ELEM_Selected|ELEM_DrawOutline) : (ELEM_Selected)), p);
                if(Hid::IsTouched(p)) {
                    menuOpt = ind;
                    currLayer = 0;
                }
                p.y += (p.h + elemSpace);
                ind++;
            }
        }
        
        //Controls 
        void IncrementSelect() {
            if(!currLayer && !MenuElements.empty()) {
                if(menuOpt < MenuElements.size()-1) {
                    menuOpt++;
                    Audio::Play(SFX_Navigate);
                } else menuOpt = (u16)MenuElements.size()-1;
            }
            else if(currLayer && !Panels.empty()) {
                Panels[menuOpt]->IncrementSelect();
            }
        }
        
		void DecrementSelect() {
            if(!currLayer && !MenuElements.empty()) {
                if(menuOpt > 0) {
                    menuOpt--;
                    Audio::Play(SFX_Navigate);
                } else menuOpt = 0;
            }
            else if(currLayer && !Panels.empty()) {
                Panels[menuOpt]->DecrementSelect();
            }
        }
        
		void Back() {
            Audio::Play(SFX_Back);
            if(!currLayer) Close();
            else currLayer = 0;
        }

		void Close() {
			Hide();
			Deinit();
		}
        
        Result Activate() {
            Result rc = 0;
            
            Audio::Play(SFX_Activate);
            if(!currLayer && MenuElements[menuOpt]->HasFunc())
                rc = MenuElements[menuOpt]->Run();
            else if(!currLayer && Panels[menuOpt]->ElementCnt() > 0) 
                currLayer = 1;
            else if(currLayer) Panels[menuOpt]->Activate();
            
            return rc;
        }
        
        Result Update() {
            Result rc = 0;
            if(!Visible) return 0;
            if(Sprite) Graphics::RenderTexture(Sprite, Pos);
            if(Title != "") Graphics::DrawText(Fonts::FONT_LARGE, 30, 25, Title);
            Draw();
            if(Panels.size() > 0) currLayer = Panels[menuOpt]->Update(currLayer);
            
            if(Hid::Input & KEY_A) rc = Activate();
            if(Hid::Input & KEY_DUP) DecrementSelect();
            if(Hid::Input & KEY_DDOWN) IncrementSelect();
            if(Hid::Input & KEY_B) Back();
            
            return rc;
        }
        
    protected:
        Rect Pos;
        u32 elemSpace;
        u16 menuOpt, currLayer;
        u32 panX, panY;
        std::string Title;
        bool Visible;
        std::vector<UIElement*> MenuElements;
        std::vector<Panel*> Panels;
        Texture *Sprite;
        UI::MessageBox *msgBox;
        UI::OpenFileDialog *ofd;
        Language *lang;
        Network *net;
};