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

#include "Engine.hpp"

EngineState Engine::State = STATE_DASHBOARD;
UI::MessageBox* UI::MessageBox::instance = nullptr;
UI::ImageOptionBox* UI::ImageOptionBox::instance = nullptr;
UI::OpenFileDialog* UI::OpenFileDialog::instance = nullptr;
Language* Language::instance = nullptr;

Engine::Engine(u32 width, u32 height) {
    //Setup vars
    screenPos.x = 0;
    screenPos.y = 0;
    screenPos.w = width;
    screenPos.h = height;
    running = true;
    
	nifmInitialize(NifmServiceType_System);
	
    //Load romfs
    if(R_FAILED(romfsMountFromFsdev("/ReiNX/titles/0100000000001000/romfs.bin", 0, "romfs")))
        if(R_FAILED(romfsMountFromFsdev("/sxos/titles/0100000000001000/romfs.bin", 0, "romfs")))
            romfsMountFromFsdev("/atmosphere/titles/0100000000001000/romfs.bin", 0, "romfs");
    
    if(!Rnx::IsUsingReiNX())
        Rnx::SetHbTidForDelta(0x010000000000100F);

}

Engine::~Engine() {
	delete frndThread;
	delete samsThread;
    delete aemThread;
    delete dash;
    romfsExit();
	nifmExit();
    Graphics::Exit();
}

void Engine::Initialize() {
    //Temp settings till i figure saves out
    Settings::SetLockIconEnabled(true);
    
    State = (Settings::GetLockScreenFlag() ? STATE_LOCKSCREEN : STATE_DASHBOARD);
    
    appletLoadAndApplyIdlePolicySettings();
    //appletAllowToEnterSleep();

	//Start threads
    frndThread = new ThreadManager(Threads::FriendThread);
    frndThread->start();
	
	samsThread = new ThreadManager(Threads::SystemAppletMessage);
	samsThread->start();
	
	aemThread = new ThreadManager(Threads::AeMessageThread);
	aemThread->start();
    
	//Init hid
	Graphics::Init(screenPos);
	Hid::TouchInit();

    //Init dashboard
    dash = new Dashboard(screenPos.w, screenPos.h);
    dash->Initialize();
}

void Engine::Render() {
    Graphics::Render();
}

void Engine::Clear() {
    Graphics::Clear();
}

void Engine::Update() {
    glfwPollEvents();
    if (glfwWindowShouldClose(GL::Window)) { running = false; return; }
    Hid::KeyProcess();
    Hid::TouchProcess();

	if (Settings::reloadTheme) {
		delete dash;
		dash = new Dashboard(screenPos.w, screenPos.h);
		dash->Initialize();
		Settings::reloadTheme = false;
	}

    switch(State) {
        case STATE_LOCKSCREEN:
        {
            dash->DrawLockScreen();
            if((Hid::Input & HidNpadButton_A) || Hid::IsTouched())
                State = STATE_DASHBOARD;
            break;
        }
        case STATE_DASHBOARD:
        {
            //Draw
            dash->DrawCanvas();
            dash->DrawButtons();
            dash->DrawGames();
            dash->DrawDebugText();
            dash->UpdateDash();
            
            //Check state
            if(dash->IsMenuOpen()) 
                State = STATE_MENU;
            else 
                State = STATE_DASHBOARD;
            
            if(dash->ReturnToLock()) {
                State = STATE_LOCKSCREEN;
                App::wakingup = false;
            }
            
            break;
        }
        case STATE_MENU:
        {
            dash->UpdateMenus();
            if(!dash->IsMenuOpen() || Settings::AddFolder)
                State = STATE_DASHBOARD;
            if(Settings::AddFolder) 
                dash->settings->Close();
            break;
        }
    }
}