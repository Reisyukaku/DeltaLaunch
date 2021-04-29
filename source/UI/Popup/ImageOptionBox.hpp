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
#include <vector>
#include "../../Core/Audio.hpp"
#include "../../Core/Graphics.hpp"
#include "../../Services/Hid.hpp"
#include "../Menus/Image.hpp"
#include "../../Core/types.h"

#define MAX_IMAGES_COLUMN 4

namespace UI {
	class ImageOptionBox {
	public:
		int Show(std::string title) {
			if (!Images.size())  return -1;
			Audio::Play(SFX_Error);
			Title = title;
			selected = 0;

			//Update form
			while (Update());

			//Clear form
			Images.clear();

			return selected;
		}

		void SetTexture(Texture *tex) {
			bgTex = tex;
			//popup area
			pos.w = bgTex->w;
			pos.h = bgTex->h;
			pos.x = (screen.w / 2) - (pos.w / 2);
			pos.y = (screen.h / 2) - (pos.h / 2);
		}

		void SetOptions(std::vector<Image*> imgs) {
			Images = imgs;
		}

		void AddOption(Texture *tex, std::string name) {
			if (!tex) return;
			Images.push_back(new Image(name, 128, 128, tex));
		}

		static ImageOptionBox* getInstance() {
			if (instance == nullptr)
				instance = new ImageOptionBox();
			return instance;
		};

	private:
		static ImageOptionBox* instance;

		ImageOptionBox() {
			//full screen area
			screen.x = screen.y = 0;
			screen.w = Graphics::GetWinWidth();
			screen.h = Graphics::GetWinHeight();
			selected = 0;
			bgTex = nullptr;
		};

		int Update() {
			Hid::KeyProcess();
			Hid::TouchProcess();

			//Draw main form
			Graphics::RenderTexture(bgTex, pos);
			Graphics::DrawText(Fonts::FONT_SMALL, pos.x + 25, pos.y + 18, Title + " | " + Images[selected]->Name, 0xFFFFFFFF, pos.w);

			//Draw buttons
			int ind = 0;
			for (auto but : Images) {
				Rect pimg = { pos.x + 25, pos.y + 80, (s32)but->Width, (s32)but->Height };
				pimg.x = (pos.x + 25) + ((ind % MAX_IMAGES_COLUMN) * (pimg.w + 12));
				pimg.y = (pos.y + 80) + ((ind / MAX_IMAGES_COLUMN) * (pimg.h + 12));
				but->Draw((ind == selected) ? ELEM_Selected : ELEM_Unselected, pimg);
				ind++;
			}

			//Hid input
			if (Hid::Input & KEY_LEFT)
				selected = (selected <= 0) ? 0 : selected - 1;
			if (Hid::Input & KEY_RIGHT)
				selected = (selected >= (int)Images.size() - 1) ? (int)Images.size() - 1 : selected + 1;
			if (Hid::Input & KEY_A) {
				Audio::Play(SFX_Activate);
				return 0;
			}
			if (Hid::Input & KEY_B) {
				selected = -1;
				return 0;
			}

			//Render
			Graphics::Render();
			return 1;
		}

		ImageOptionBox(const ImageOptionBox&);
		ImageOptionBox& operator=(const ImageOptionBox&);
		Rect pos;
		Rect screen;
		int selected;
		Texture *bgTex;
		std::string Title;
		std::vector<Image*> Images;
	};
};