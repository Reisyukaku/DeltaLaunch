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
#include <tuple>
//#include <dirent.h>
#include <filesystem>

#include "Threads.hpp"
#include "ThreadManager.hpp"
#include "../Core/Graphics.hpp"
#include "../Core/Audio.hpp"
#include "../UI/Menus/Menu.hpp"
#include "../UI/Language.hpp"
#include "../UI/Dashboard.hpp"
#include "../UI/Forms.hpp"
#include "../UI/Game.hpp"
#include "../Services/Hid.hpp"
#include "../Services/App.hpp"
#include "../Services/Power.hpp"
#include "../Services/Settings.hpp"
#include "../Services/Network.hpp"
#include "../Core/types.h"

enum EngineState {
	STATE_LOCKSCREEN,
	STATE_DASHBOARD,
	STATE_MENU
};

class Engine
{
    public:
        Engine(u32 width, u32 height);
        ~Engine();
        void Initialize();
        void Render();
        void Clear();
        void Update();
		
        Dashboard *dash;
        static EngineState State;
        bool running;
    private:
        ThreadManager *frndThread;
		ThreadManager *samsThread;
		ThreadManager *aemThread;
        Rect screenPos; 
};