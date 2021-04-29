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
#include "./Menus/Menu.hpp"
#include "Forms.hpp"
#include "Language.hpp"
#include "../Services/App.hpp"
#include "../Services/Account.hpp"
#include "../Core/Utils.hpp"
#include "../Core/Graphics.hpp"
#include "../Core/types.h"

#define SAVE_PATH SD("/save_backups")

namespace fs = std::filesystem;

class GameBase {
public:
	GameBase(u64 tid = 0) {
		msgBox = UI::MessageBox::getInstance();
		lang = Language::getInstance();
		TitleID = tid;
		Icon = nullptr;
		inFolder = false;
		menu = nullptr;
	};

	virtual ~GameBase() {
		//
	};

	virtual Result Run() { return 0; };
	virtual bool IsRunning() { return false; }

	//Getters/setters
	virtual std::string GetAuthor() { return Author; }
	virtual void SetAuthor(std::string author) { Author = author; }
	virtual std::string GetName() { return Name; }
	virtual void SetName(std::string name) { Name = name; }
	virtual u8 GetType() { return Type; }
	virtual void SetType(u8 type) { Type = type; }
	virtual u64 GetTitleID() { return TitleID; }
	virtual void SetTitleID(u64 tid) { TitleID = tid; }
	virtual std::string GetVersion() { return Version; }
	virtual void SetVersion(std::string ver) { Version = ver; }
	virtual Texture *GetIcon() { return Icon; }
	virtual void SetIcon(Texture *tex) { Icon = tex; }

	virtual void SetInFolder(bool b) { inFolder = b; }
	virtual bool IsInFolder() { return inFolder; }
	virtual bool IsFolder() { return false; }

	virtual Result DrawOptions() { return 0; }
	Result ShowOptions() {
		Result rc = DrawOptions();
		if (Hid::Input & KEY_B) {
			//delete menu;
			//menu = nullptr;
			rc = 1;
		}
		return rc;
	}

	void SetOptionsTexture(Texture *tex) {
		OptTex = tex;
		OptPos = { (float)((Graphics::GetWinWidth() / 2.0f) - (tex->w / 2.0f)), (float)((Graphics::GetWinHeight() / 2.0f) - (tex->h / 2.0f)), (float)tex->w, (float)tex->h };
	}

protected:
	u64 TitleID;
	std::string Name;
	std::string Author;
	std::string Version;
	u8 Type;
	bool inFolder;
	Rect OptPos;
	Texture *OptTex;
	Texture *Icon;
	Menu *menu;
	Language *lang;
	UI::MessageBox *msgBox;
};