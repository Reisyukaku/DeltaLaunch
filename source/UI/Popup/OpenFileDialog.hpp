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
#include <filesystem>
#include "../../Core/Audio.hpp"
#include "../../Core/Graphics.hpp"
#include "../../Services/Hid.hpp"
#include "../../Core/types.h"

namespace UI {
	class OpenFileDialog {
	public:
		std::filesystem::path Show(std::string title = "Open file", std::string startDir = SD("/")) {
			Audio::Play(SFX_Error);
			Title = title;
			selected = 0;
			PopulatePaths(startDir);

			//Update form
			while (Update());

			Filter = "";

			return Fil;
		}

		void SetFilter(std::string filter) {
			Filter = filter;
		}

		void SetTexture(Texture *tex) {
			bgTex = tex;
			pos = { ((screen.w / 2.0f) - (bgTex->w / 2.0f)), (float)((screen.h / 2.0f) - (bgTex->h / 2.0f)), (float)bgTex->w, (float)bgTex->h };
		}

		static OpenFileDialog* getInstance() {
			if (instance == nullptr)
				instance = new OpenFileDialog();
			return instance;
		};

	private:
		static OpenFileDialog* instance;

		OpenFileDialog() {
			//full screen area
			screen.x = screen.y = 0;
			screen.w = Graphics::GetWinWidth();
			screen.h = Graphics::GetWinHeight();
			bgTex = nullptr;
		};

		int Update() {
			Hid::KeyProcess();
			Hid::TouchProcess();

			//Draw main form
			Graphics::RenderTexture(bgTex, pos);
			Graphics::DrawText(Fonts::FONT_SMALL, pos.x + 25, pos.y + 18, Title, 0xFFFFFFFF, pos.w);

			//Draw options
			int ind = 0, space = 0, offset = 0;
			for (auto p : paths) {
				if (selected >= 15)
					offset = (30 * (selected - 15));
				std::string opt = ((ind == selected) ? "> " : "  ") + p.filename().string() + (std::filesystem::is_directory(p) ? "/" : "");
				if (pos.y + 80 + space - offset >= pos.y + 80 && pos.y + 80 + space - offset <= pos.y + 80 + (15 * 30))
					Graphics::DrawText(Fonts::FONT_SMALL, pos.x + 25, pos.y + 80 + space - offset, opt, (ind == selected) ? Graphics::GetSelectCol() : RGBA(0xFFFFFFFF), pos.w);
				space += 30;
				ind++;
			}

			//Hid input
			if (Hid::Input & KEY_DUP)
				selected = (selected <= 0) ? 0 : selected - 1;
			if (Hid::Input & KEY_DDOWN)
				selected = (selected >= (int)paths.size() - 1) ? (int)paths.size() - 1 : selected + 1;
			if (Hid::Input & KEY_A) {
				Result rc;
				Audio::Play(SFX_Activate);
				if (std::filesystem::is_directory(paths[selected])) {
					PopulatePaths(paths[selected]);
					rc = 1;
				}
				else {
					Fil = paths[selected].string();
					rc = 0;
				}
				return rc;
			}
			if (Hid::Input & KEY_B) {
				Fil = std::filesystem::path("");
				return 0;
			}

			//Render
			Graphics::Render();
			return 1;
		}

		void PopulatePaths(std::filesystem::path path) {
			if (paths.size() > 0) paths.clear();
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				if (Filter == "" || entry.path().extension() == Filter) paths.push_back(entry.path());
			}
			selected = 0;
		}

		OpenFileDialog(const OpenFileDialog&);
		OpenFileDialog& operator=(const OpenFileDialog&);
		Rect pos;
		Rect screen;
		int selected;
		Texture *bgTex;
		std::string Title;
		std::string Filter;

		std::filesystem::path Fil;
		std::vector<std::filesystem::path> paths;
	};
};