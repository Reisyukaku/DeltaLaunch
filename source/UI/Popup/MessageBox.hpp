/*
* ReiNX Toolkit
* Copyright (C) 2018  Reisyukaku
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <string>
#include <vector>
#include "../../Core/Audio.hpp"
#include "../../Core/Graphics.hpp"
#include "../../Services/Hid.hpp"
#include "../Menus/Button.hpp"
#include "../Language.hpp"
#include "../..//Core/types.h"

enum MsgType {
    MSGBOX_OK,
    MSGBOX_YESNO,
};

enum DialogResult {
    RESULT_OK,
    RESULT_ABORT,
    RESULT_NO,
    RESULT_YES
};

namespace UI {
	class MessageBox {
	public:
		DialogResult Show(std::string title, std::string body, MsgType type = MSGBOX_OK) {
			Audio::Play(SFX_Error);
			Title = title;
			Body = body;
			msgType = type;
			selected = 0;
			MsgResult = RESULT_ABORT;

			//Create buttons
			switch (type) {
                case MSGBOX_OK:
                {
                    Buttons.push_back(Button(lang->GetString(LANG_OK), [&]()->Result { return MSGBOX_OK; }));
                    break;
                }
                case MSGBOX_YESNO:
                {
                    Buttons.push_back(Button(lang->GetString(LANG_NO), [&]()->Result { return RESULT_NO; }));
                    Buttons.push_back(Button(lang->GetString(LANG_YES), [&]()->Result { return RESULT_YES; }));
                    break;
                }
			}

			//Update form
			while (Update());

			//Clear form
			Buttons.clear();

			return MsgResult;
		}

		void SetTexture(Texture *tex) {
			bgTex = tex;
		}

		static MessageBox* getInstance() {
			if (instance == nullptr)
				instance = new MessageBox();
			return instance;
		};

	private:
		static MessageBox* instance;

		MessageBox() {
			//full screen area
			screen.x = screen.y = 0;
			screen.w = Graphics::GetWinWidth();
			screen.h = Graphics::GetWinHeight();

			//popup area
			pos.w = screen.w / 2;
			pos.h = screen.h / 2;
			pos.x = (screen.w / 2) - (pos.w / 2);
			pos.y = (screen.h / 2) - (pos.h / 2);

			butW = 150.0f;
			butH = 50.0f;
			bgTex = nullptr;

			lang = Language::getInstance();

			selected = 0;
		};

		int Update() {

			Hid::KeyProcess();
			Hid::TouchProcess();

			//Draw main form
			Graphics::RenderTexture(bgTex, pos);
			Graphics::DrawText(Fonts::FONT_SMALL, pos.x + 25, pos.y + 18, Title, 0xFFFFFFFF/*Graphics::GetMessageCol()*/, pos.w);
			Graphics::DrawText(Fonts::FONT_SMALL, pos.x + 25, pos.y + 80, Body, Graphics::GetMessageCol(), pos.w);

			//Draw buttons
			int ind = 0;
			for (auto but : Buttons) {
				but.Draw((ind == selected) ? (ElementType)(ELEM_Selected | ELEM_DrawOutline) : ELEM_DrawOutline, { (float)(pos.x + pos.w - ((butW + 25.0f) * (ind + 1))), (float)(pos.y + pos.h - butH - 25.0f), (float)butW, (float)butH });
				ind++;
			}

			//Hid input
			if (Hid::Input & KEY_LEFT)
				selected = (selected >= Buttons.size() - 1) ? (u8)Buttons.size() - 1 : selected + 1;
			if (Hid::Input & KEY_RIGHT)
				selected = (selected == 0) ? 0 : selected - 1;
			if (Hid::Input & KEY_A) {
				Audio::Play(SFX_Activate);
				MsgResult = (DialogResult)Buttons[selected].Run();
				return 0;
			}

			//Render
			Graphics::Render();
			return 1;
		}

		MessageBox(const MessageBox&);
		MessageBox& operator=(const MessageBox&);
		Rect pos;
		Rect screen;
		u8 selected;
		float butW, butH;
		Texture *bgTex;
		MsgType msgType;
		std::string Title, Body;
		std::vector<Button> Buttons;
		Language* lang;
		DialogResult MsgResult;
	};
};