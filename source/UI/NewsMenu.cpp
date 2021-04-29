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

#include "NewsMenu.hpp"
#include "../Core/Utils/JSON.hpp"

NewsMenu::NewsMenu(Rect pos, Texture *bg, Texture *article) : Menu(pos, bg, LANG_NEWS){
	panX = 450;
    panY = 100;
	Article = article;
}

NewsMenu::~NewsMenu() {
    //
}

void NewsMenu::Initialize() {
	//Get news
	std::string news = net->Request("https://reinx.guide/api/news?_format=json", HTTP_GET);

	u32 imgW = 256, imgH = 128;
	rapidjson::Document d;
	d.Parse((char*)news.c_str());
	if (d.IsArray()) {
		for (auto itr = d.Begin(); itr != d.End(); ++itr)
		{
			if (itr->HasMember("title") && (*itr)["title"].IsString())
			{
				std::string title = (*itr)["title"].GetString();
				std::string body = (*itr)["body"].GetString();
				Panel *article = new Panel(panX, panY);
				MenuElements.push_back(new Image("News", imgW, imgH, Article, OpenNews));
				article->AddString(title);
				article->AddString("");
				std::string bod = "";
				int i = 0; for (char &c : body) {
					if (c == '\n') {
						article->AddString(bod);
						continue;
					}
					else {
						bod.insert(i++, 1, c);
					}
					if (i >= 50 && c == ' ') {
						article->AddString(bod);
						i = 0;
						bod = "";
					}
				}
				Panels.push_back(article);
			}
		}
	}
}

void NewsMenu::Deinit() {
	for (auto &pan : Panels) delete pan;
	for (auto &mel : MenuElements) delete mel;
	Panels.clear();
	MenuElements.clear();
}

void NewsMenu::Draw() {
    int ind = 0;
    Rect p = Pos;
    p.y += MENU_BUTTON_START_Y;
    p.x += MENU_BUTTON_INDENT;
    for(auto button: MenuElements) {
        p.w = static_cast<Image*>(button)->Width;
        p.h = static_cast<Image*>(button)->Height;
        button->Draw(ind == menuOpt ? ELEM_Selected : ELEM_Unselected, p);
        if(Hid::IsTouched(p)) {
            menuOpt = ind;
            currLayer = 0;
        }
        p.y += (p.h + elemSpace);
        ind++;
    }
    Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + Pos.w - 260, Pos.y + Pos.h - 50, std::string(FON_B) + " " + lang->GetString(LANG_BACK));
    Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + Pos.w - 130, Pos.y + Pos.h - 50, std::string(FON_A) + " " + lang->GetString(LANG_OK));
}