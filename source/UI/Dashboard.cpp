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

#include "Dashboard.hpp"

Dashboard::Dashboard(u32 width, u32 height) {
    WindowPos.x=WindowPos.y=0; 
    WindowPos.w=width; WindowPos.h=height;
    
    msgBox = UI::MessageBox::getInstance();
    optBox = UI::ImageOptionBox::getInstance();
    ofd = UI::OpenFileDialog::getInstance();
    lang = Language::getInstance();
    
    //vars
	App::dashLayer = 0;
	App::gameSelectInd = 0;
    lastErr = 0;
    lastMode = (SelectType)-1;
    debugInfo = false;
    draggingGameArea = false;
    gameOptionsActive = false;
    MaxColumns = 12;
    gamePicSize = 0, gameSpacing = 0;
    settings = nullptr;
    news = nullptr;
    folderIcon = nullptr;
	Wallpaper = nullptr;
	LockScreen = nullptr;
	Battery = nullptr;
	formTex = nullptr;
	dummyGame = nullptr;
	menuTex = nullptr;
    
    //Init delta save data
    mINI::INIFile file(SD("/ReiNX/deltaSave.ini"));
    mINI::INIStructure ini;
    if(file.read(ini) == false || ini.size() == 0u) {
        ini["SaveData"]["theme"] = "";
        file.generate(ini);
    }
}

Dashboard::~Dashboard() {
    delete settings;
    delete news;
    for(auto &button: AppletIcons) delete button;
    for(auto &game: GameEntries) delete game;
    AppletIcons.clear();
    GameEntries.clear();
	Graphics::FreeTexture(menuTex);
    Graphics::FreeTexture(Wallpaper);
    Graphics::FreeTexture(LockScreen);
    Graphics::FreeTexture(Battery);
	Graphics::FreeTexture(folderIcon);
	Graphics::FreeTexture(formTex);
	Graphics::FreeTexture(newsTex);
	Graphics::DestroyShaders();
}

/*
*   Initialize stuff
*/
void Dashboard::Initialize() {
	DEBUG(Debug::Type::DASHBOARD, "Init.\n");
    //Create buttons to add to dash
    u32 x = 230;       //padding on edges
    u32 space = 100;   //space inbetween
    Settings::LoadSavedTheme();
    std::string thmPath = Settings::GetThemePath();
    mINI::INIFile file(!thmPath.empty() ? (thmPath + ".cfg") : "");
    mINI::INIStructure ini;
    file.read(ini);

    //Get theme categories if exist
	DEBUG(Debug::Type::DASHBOARD, "Parse theme ini\n");
    auto snd = ini.get("Sound");
    auto gen = ini.get("General");
    auto batOv = ini.get("BatteryOverlay");
    auto clkOv = ini.get("ClockOverlay");
    auto bg = ini.get("Background");
    auto folder = ini.get("Folders");

	//Init gfx shaders
	Graphics::SetupShaders(gen.getStr("vert_shader", RESOURCES("/Shaders/default.vsh"), thmPath + "/"), gen.getStr("frag_shader", RESOURCES("/Shaders/default.fsh"), thmPath + "/"));

	//Label it default
	if (!Settings::currentTheme.length())
		Settings::currentTheme = lang->GetString(LANG_NONE);
    
    //Setup audio and grahpics
    Audio::Init(snd.getDouble("bgm_vol", 64.0));
    Audio::SetSfx(snd.getStr("sfx_navigate", RESOURCES("/Sounds/menu_nav.mp3"), thmPath+"/"), SFX_Navigate);
    Audio::SetSfx(snd.getStr("sfx_activate", RESOURCES("/Sounds/menu_activate.mp3"), thmPath+"/"), SFX_Activate);
    Audio::SetSfx(snd.getStr("sfx_back", RESOURCES("/Sounds/menu_back.mp3"), thmPath+"/"), SFX_Back);
    Audio::SetSfx(snd.getStr("sfx_error", RESOURCES("/Sounds/sfx_error.mp3"), thmPath+"/"), SFX_Error);
    Audio::SetSfx(snd.getStr("sfx_bgm", "", thmPath+"/"), SFX_Bgm);
    
    //Setup text colors
    Graphics::SetSelectCol(RGBA(gen.getU32("selection_col", 0x00FFCEFF)));
    Graphics::SetMessageCol(RGBA(gen.getU32("message_col", 0xFFFFFFFF)));
    Graphics::SetButtonCol(RGBA(gen.getU32("button_col", 0x4C4C4CFF)));
    Graphics::SetHeaderCol(RGBA(gen.getU32("header_col", 0xFFFFFFFF)));
    
    //Display lockscreen icon bool
    Settings::SetLockIconEnabled(gen.getBool("lock_icon", true));
    
    //Define applet buttons
    std::vector<std::tuple<std::string, std::function<Result()>>> ButtonFuncs{
        std::tuple<std::string, std::function<Result()>>{"WebButton", std::bind(App::LaunchWebsite, "https://google.com/")},
        std::tuple<std::string, std::function<Result()>>{"NewsButton", std::bind(&Dashboard::OpenNews, this)},
        std::tuple<std::string, std::function<Result()>>{"ShopButton", App::LaunchShop},
        std::tuple<std::string, std::function<Result()>>{"AlbumButton", std::bind(App::LaunchAlbum, 2, true)}, //1,false | 2,true
        std::tuple<std::string, std::function<Result()>>{"HomebrewButton", App::LaunchHbl},
        std::tuple<std::string, std::function<Result()>>{"SettingsButton", std::bind(&Dashboard::OpenSettings, this)},
        std::tuple<std::string, std::function<Result()>>{"SleepButton", Power::Sleep},
    };
    
    //Clear buttons if any exist
    if(AppletIcons.size() > 0) {
        for(auto &button: AppletIcons) delete button;
        AppletIcons.clear();
    }
    
    //Create buttons
    for(u32 but = 0; but < ButtonFuncs.size(); but++) {
        auto butt = ini.get(std::get<0>(ButtonFuncs.at(but)));
        if(butt.getBool("disabled")) continue;
        AppletIcons.push_back(new AppletIcon(
            butt.getStr("sprite", RESOURCES("/Graphics/Icons/") + std::get<0>(ButtonFuncs.at(but)) + ".png", thmPath+"/"),
            butt.getStr("sprite_select", RESOURCES("/Graphics/Icons/") + std::get<0>(ButtonFuncs.at(but)) + "_select.png", thmPath+"/"),
            butt.getInt("x", x+=space),
            butt.getInt("y", 600),
            std::get<1>(ButtonFuncs.at(but))
        ));
    }
    
    //Sort buttons based on X coord
    std::sort( AppletIcons.begin(), AppletIcons.end(), []( const AppletIcon *lhs, const AppletIcon *rhs )
    {
       return lhs->Pos.x < rhs->Pos.x;
    });
    
    //Setup folder sprite
    folderIcon = Graphics::CreateTexFromString(folder.getStr("icon", RESOURCES("/Graphics/Folder.png"), thmPath+"/"));
	menuTex = Graphics::CreateTexFromString(gen.getStr("menus", RESOURCES("/Graphics/Forms/Menu.png")));
    
    //Init settings menu
	DEBUG(Debug::Type::DASHBOARD, "Creating settings\n");
    settings = new SettingsMenu(WindowPos, menuTex);
    
    //Init news menu
	DEBUG(Debug::Type::DASHBOARD, "Creating news\n");
	newsTex = Graphics::CreateTexFromString(RESOURCES("/Graphics/reinxnews.png"));
    news = new NewsMenu(WindowPos, menuTex, newsTex);
    
    //Set popup textures
	formTex = Graphics::CreateTexFromString(gen.getStr("popup_form", RESOURCES("/Graphics/Forms/PopupForm.png"), thmPath + "/"));
    optBox->SetTexture(formTex);
    ofd->SetTexture(formTex);

	msgTex = Graphics::CreateTexFromString(gen.getStr("popup_message", RESOURCES("/Graphics/Forms/PopupMessage.png"), thmPath + "/"));
	msgBox->SetTexture(msgTex);
    
    //Setup dash wallpaper
    std::vector<std::string> layers;
    std::string tmp;
    int lay;for(lay=0; lay < BACKGROUND_LAYERS; lay++) {
        tmp = bg.getStr("layer"+ std::to_string(lay), "", thmPath+"/");
        if(lay == 0 && tmp == "") tmp = RESOURCES("/Graphics/Wallpaper.png");
        if(tmp == "") continue;
        layers.push_back(tmp);
    }
    
    //Create new game objects
    std::vector<NsApplicationRecord> recs;
    if(GameEntries.size() > 0) GameEntries.clear();
    App::GetAppRecords(recs);
    for(u32 i = 0; i < recs.size(); i++){
        GameEntries.push_back(new Game());
    }
    
    recs.clear();
    GameIconArea.x = 120;
    GameIconArea.y = gen.getInt("gamearea_y", 200);
    
    //Set default layout
    switch(gen.getInt("default_gamelayout", 0)) {
        default:
        case 0: Settings::gameSelType = SELECT_OUTLINE; break;
        case 1: Settings::gameSelType = SELECT_SIZEDIFF; break;
        case 2: Settings::gameSelType = SELECT_PSSTYLE; break;
    }
    
    //Clear and setup overlay and dash
	DEBUG(Debug::Type::DASHBOARD,"Setting up overlay\n");
    BatPos.x = batOv.getInt("x", 1080);
    BatPos.y = batOv.getInt("y", 14);
    ClkPos.x = clkOv.getInt("x", 1150);
    ClkPos.y = clkOv.getInt("y", 14);
    Wallpaper = Graphics::BlitSurfacesFromString(layers, WindowPos);
    SetLockScreen(gen.getStr("lockscreen_image", RESOURCES("/Graphics/LockSplash.png"), thmPath+"/"), RESOURCES("/Graphics/Overlay/LockIcon.png"));
    Battery = Graphics::CreateTexFromString(batOv.getStr("battery", RESOURCES("/Graphics/Overlay/Battery.png"), thmPath+"/"));
#ifndef __SWITCH__
	dummyGame = Graphics::CreateTexFromString(RESOURCES("/Graphics/DummyGame.png"));
#endif
    layers.clear();
    
    //Organize games and add saved folders
    SetGames();
    PopulateFolders();
    
    //Start BG music if applicable
    Audio::Play(SFX_Bgm, true);
}

void Dashboard::SetGames() {
	DEBUG(Debug::Type::DASHBOARD, "Setting up games\n");
    std::string thmPath = Settings::GetThemePath();
    mINI::INIFile file(!thmPath.empty() ? (thmPath + ".cfg") : "");
    mINI::INIStructure thm;
    file.read(thm);
    
    switch(Settings::gameSelType) {
        case SELECT_OUTLINE: { gamePicSize = 256; gameSpacing = 14; gameRows = 1; break; }
        case SELECT_SIZEDIFF: { gamePicSize = 256; gameSpacing = 40; gameRows = 1; break; }
        case SELECT_PSSTYLE: { gamePicSize = 128; gameSpacing = 5; gameRows = 1; break; }
        case SELECT_DOUBLEROW: { gamePicSize = 128; gameSpacing = 5; gameRows = 2; break; }
    }
    App::dashLayer = gameRows;
    
    GameIconArea.w = (gamePicSize+gameSpacing)*MaxColumns;
    GameIconArea.h = gamePicSize;
	
    GameSelect = GameIconArea;
    GameSelect.x = (GameIconArea.x + (App::gameSelectInd * gamePicSize));
    GameSelect.w = GameSelect.h = gamePicSize;
    
    //Create game images
    std::vector<NsApplicationRecord> recs;
    App::GetAppRecords(recs);
    int i = 0;
    size_t total = recs.size();
    
    //Define game objects
    for(auto game: GameEntries) {
        u64 tid = i < (int)total ? recs[i].application_id : 0;
        if(!game->IsFolder() && tid != game->GetTitleID()) { //assume game entry doesnt need to be updated if tids are the same
            game->SetTitleID(tid);
            game->SetType(recs[i].type);
            
            NsApplicationControlData data = App::GetGameControlData(tid, NsApplicationControlSource_Storage);
            if(game->GetIcon())
                Graphics::FreeTexture(game->GetIcon());
            std::string gicon = thm.get("TitleIcon").getStr(Utils::NumToHexStr(game->GetTitleID()), "", thmPath+"/");
            if(gicon != "") {
                game->SetIcon(Graphics::CreateTexFromString(gicon));
            } 
            else {
#ifdef __SWITCH__
                game->SetIcon(Graphics::CreateTexFromBuffer(data.icon, 0x20000));
#else
				game->SetIcon(dummyGame); //only one of these exists so dont free more than one
#endif
            }
#ifdef __SWITCH__
            NacpLanguageEntry *langEnt = NULL;
            nacpGetLanguageEntry(&data.nacp, &langEnt);
            game->SetName(langEnt->name);
            game->SetAuthor(langEnt->author);
			game->SetVersion(std::string(data.nacp.display_version));
#else
			game->SetName(data.nacp.lang[0].name);
			game->SetAuthor(data.nacp.lang[0].author);
			game->SetVersion("0.0");
#endif
            game->SetOptionsTexture(formTex);
        }
        i++;
    }
    
    recs.clear();
}

void Dashboard::PopulateFolders() {
    //Load save
    mINI::INIFile file(SD("/ReiNX/deltaSave.ini"));
    mINI::INIStructure save;
    file.read(save);
    
   //Sort games to folders
    for(int i = 0; i < MAX_FOLDERS; i++) {
        std::string defName = "Folder"+std::to_string(i);
        if(!save.has(defName)) continue;
        
        GameFolder *folder = new GameFolder(i, save.get(defName).getStr("name", defName), folderIcon);
		folder->SetOptionsTexture(formTex);

        int ind = 0;
        for(auto game : GameEntries){
            if(game->IsFolder() && save.get(defName).has(std::to_string(game->GetTitleID()))){
                if(folder->Games.size() < MAX_GAMES_PER_FOLDER) {
                    folder->AddGame(game->GetTitleID(), game->GetIcon(), game->GetName());
                }
            }
            ind++;
        }
        
        GameEntries.push_back(folder);
    }
}

/*
*   Update logic
*/
void Dashboard::UpdateDash() {
    //Update on style change
    if(lastMode != Settings::gameSelType) {
        lastMode = Settings::gameSelType;
        SetGames();
    }
    //Keep layer in bounds
    if(App::dashLayer > gameRows) 
        App::dashLayer = gameRows;
    
    //Normal dash controls
    if(!gameOptionsActive) {
        if(Hid::Input & KEY_A) ActivateDash();
        if(Hid::Input & KEY_X) CloseApp();
        if(Hid::Input & KEY_LSTICK)  debugInfo = !debugInfo;
		if (Hid::Input & KEY_MINUS) {}
        if(Hid::Input & KEY_PLUS) gameOptionsActive = true;
        if((Hid::Input & KEY_DLEFT) || (Hid::Input & KEY_LSTICK_LEFT)) DecrementDashSel();
        if((Hid::Input & KEY_DRIGHT) || (Hid::Input & KEY_LSTICK_RIGHT)) IncrementDashSel();
        if((Hid::Input & KEY_DUP) || (Hid::Input & KEY_LSTICK_UP)) IncrementLayer();
        if((Hid::Input & KEY_DDOWN) || (Hid::Input & KEY_LSTICK_DOWN)) DecrementLayer();
        
        draggingGameArea = Hid::IsMoving(GameIconArea);
        if(draggingGameArea) {
            if(lastPosX != 0) 
                OffsetGameIcons(Hid::GetTouchPos().px - lastPosX);
            lastPosX = Hid::GetTouchPos().px;
        } else {
            lastPosX = 0;
        }
    } 
    //Game option controls (dont want to halt thread)
    else {
        if(GameEntries[App::gameSelectInd]->ShowOptions()) gameOptionsActive = false;
    }
    
    CheckEvents();
}

void Dashboard::CheckEvents() {
    //Check if folder needs to be added
    if(Settings::AddFolder) {
        AddFolder();
        Settings::AddFolder = false;
    }
    
    if(Settings::deleteFolder != -1) {
        DeleteFolder(Settings::deleteFolder);
        Settings::deleteFolder = -1;
    }
    
    if(Settings::addToFolder) {
        for(auto f : GameEntries) {
            if (f->IsFolder()) optBox->AddOption(f->GetIcon(), f->GetName());
        }
        s32 id = optBox->Show("Select folder");
        if(id != -1) MoveSelectedGameToFolder(id);
        Settings::addToFolder = false;
    }
    
    //SD card cant be removed
    if(App::sdRemoved) {
        msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_REMOVE_SD));
        Power::Reboot();
    }
    
    /*if(App::IsGamecardInserted() == GcState) {
        SetGames();
        GcState = !GcState;
    }*/
}

/*
*   Drawing dashboard
*/
void Dashboard::DrawGames() {
    Rect gameArea = GameIconArea;
    gameArea.w = gamePicSize;
    GameSelect.x = GameIconArea.x + (App::gameSelectInd * (gamePicSize + gameSpacing));
    
    //Make sure selected game isnt off screen
    if(GameSelect.x < 0) 
        OffsetGameIcons(120 - GameSelect.x);
    if(GameSelect.x > (1280 - GameSelect.w - 120)) 
        OffsetGameIcons(-(GameSelect.w - (1280 - GameSelect.x)) - 120);
    
    //Draw games
    int i = 0;
    for(auto entry : GameEntries) {
		if (entry->IsInFolder()) continue;
        gameArea.x = GameIconArea.x + (i * (gamePicSize+gameSpacing));
        bool isPlaying = (App::currentApplication.active == true && App::currentApplication.tid == entry->GetTitleID());
        switch(Settings::gameSelType) {
            //Classic outline format
            default:
            case SELECT_OUTLINE:
            {
                //Draw selection outline
                if(i == App::gameSelectInd && App::dashLayer > 0) {
                    Rect pos = GameSelect; 
                    pos.x -= 5; pos.y -= 5; pos.w += 10; pos.h += 10;
                    Graphics::Rectangle(pos, Graphics::GetSelectCol());
                    
                    if(gameRows == 1) Graphics::DrawText(Fonts::FONT_SMALL, pos.x, pos.y-25, entry->GetName() + (isPlaying ? std::string(" [") + lang->GetString(LANG_PLAYING) + std::string("]") : ""), Graphics::GetSelectCol());
                }
                
                //Draw either game icon or backer
                if(entry->GetIcon()) {
                    Graphics::RenderTexture(entry->GetIcon(), gameArea, nullptr, true);
                } 
                else {
                    Graphics::Rectangle(gameArea, 0x70);
                }
                
                if(!IsMenuOpen() && !gameOptionsActive && Hid::IsTouched(gameArea))
                    SelectGame(entry, i);
                
                break;
            }
            //Diff sizes; selected is highlighted
            case SELECT_SIZEDIFF:
            {
                //Draw game bigger
                if(i == (int)App::gameSelectInd && App::dashLayer > 0) {
                    Rect pos = GameSelect; 
                    pos.x -= 20; pos.y -= 20; pos.w += 40; pos.h += 40;
                    Graphics::RenderTexture(entry->GetIcon(), pos, nullptr, true);
                }
                //Draw either game icon or backer
                else {
                    if(entry->GetIcon()) {
                        Graphics::RenderTexture(entry->GetIcon(), gameArea, nullptr, true);
                    }
                    Graphics::Rectangle(gameArea, 0x70);
                }
                
                if(!IsMenuOpen() && !gameOptionsActive && Hid::IsTouched(gameArea)) 
                    SelectGame(entry, i);
                
                break;
            }
            //PS4 style
            case SELECT_PSSTYLE:
            {
                //Draw selection outline
                if(i == (int)App::gameSelectInd && App::dashLayer > 0) {
                    Rect pos = GameSelect; 
                    pos.h = ((GameSelect.h * 2) + 40);
                    pos.w = (GameSelect.w * 2);
                    //selection
                    pos.w += 10; pos.h += 10;
                    pos.x -= 5; pos.y -= 5;
                    Graphics::Rectangle(pos, Graphics::GetSelectCol());
                    //backer
                    pos = GameSelect;
                    pos.h = ((GameSelect.h * 2) + 40);
                    pos.w = (GameSelect.w * 2);
                    Graphics::Rectangle(pos, 0x666666FF);
                    //icon
                    pos.h = (GameSelect.h * 2);
                    Graphics::RenderTexture(entry->GetIcon(), pos, nullptr, true);
                    
                    Graphics::DrawText(Fonts::FONT_SMALL, pos.x + 105, pos.y + pos.h + 8, (entry->IsFolder()) ? lang->GetString(LANG_OPEN) : (isPlaying ? lang->GetString(LANG_PLAYING) : lang->GetString(LANG_START)), 0xFFFFFFFF);
                    if(gameRows == 1) Graphics::DrawText(Fonts::FONT_SMALL, pos.x + pos.w + 20, pos.y + pos.h + 8, entry->GetName(), Graphics::GetSelectCol());
                    
                    if(!IsMenuOpen() && !gameOptionsActive && Hid::IsTouched(GameSelect)) 
                        SelectGame(entry, i);
                }
                else {
                    Rect pos = gameArea;
                    //Draw either game icon or backer
                    if(i >= App::gameSelectInd && App::dashLayer > 0) pos.x += 128;
                    if(entry->GetIcon()) {
                        Graphics::RenderTexture(entry->GetIcon(), pos, nullptr, true);
                    } 
                    else {
                        Graphics::Rectangle(pos, 0x70);
                    }
                    
                    if(!IsMenuOpen() && !gameOptionsActive && Hid::IsTouched(pos)) 
                        SelectGame(entry, i);
                }
                
                break;
            }
        }
        i++;
    }
}

void Dashboard::DrawCanvas() {
    //Draw wallpaper
    Graphics::RenderTexture(Wallpaper, WindowPos);
    
    //Draw overlay
    BatPos.w = 53*6; BatPos.h = 25;
    Rect spritePos = BatPos;
    spritePos.w = BatPos.w/6;
    spritePos.h = spritePos.h;
    u32 index = (Power::GetBatteryLife()/20);
    spritePos.x = spritePos.w * (index >= 5 ? 4 : index);
    spritePos.y = 0;
    
    Graphics::RenderTexture(Battery, BatPos, &spritePos);
    Graphics::DrawText(Fonts::FONT_SMALL, ClkPos.x, ClkPos.y, Time::GetClock(Time::CLOCK_12HR), 0xFFFFFFFF);
}

void Dashboard::DrawLockScreen() {
    Rect lock;
    //100x100 icon seems nice
    u32 lckW = 150, lckH = 150;
    lock.x = (WindowPos.w/2)-(lckW/2);
    lock.y = (WindowPos.h/2)-(lckH/2);
    lock.w = lckW;
    lock.h = lckH;
    
    Graphics::RenderTexture(LockScreen, WindowPos);
    Graphics::DrawText(Fonts::FONT_MEDIUM, 25, 570, Time::GetDate(), 0xFFFFFFFF);
    Graphics::DrawText(Fonts::FONT_HUGE, 25, 600, Time::GetClock(Time::CLOCK_12HR), 0xFFFFFFFF);
    Graphics::RenderTexture(LockIcon, lock);
}

void Dashboard::SetLockScreen(std::string image, std::string lock) {
    LockScreen = Graphics::CreateTexFromString(image);
    LockIcon = Graphics::CreateTexFromString(lock);
}

void Dashboard::DrawButtons() {
    s32 ind = 0;
    for(auto button: AppletIcons) {
        //Render icon/selected icon
        if(button->Sprite)
            button->Draw((ind == App::appletSelectInd && App::dashLayer == 0) ? ELEM_Selected : ELEM_Unselected);
        
        //Touch detect
        if((Hid::IsTouched(button->Pos)) && !IsMenuOpen() && !gameOptionsActive) {
            if(App::appletSelectInd == ind && App::dashLayer == 0) {
                lastErr = button->Run();
                if(lastErr) 
                    msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_ERROR_CODE) + ": " + std::to_string(lastErr));
				App::RequestForegroundDelta();
            } else {
                App::appletSelectInd = ind;
                App::dashLayer = 0;
            }
        }
        ind++;
    }
}

void Dashboard::DrawDebugText() {
    if(debugInfo) {
        u32 X = 14,  Y = 0, s = 14;
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "DeltaLaunch alpha!");
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "Firmware: " + Settings::GetFirmwareVersion());
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "Battery: " + std::to_string(Power::GetBatteryLife()) + "%");
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "Touch: X=" + std::to_string(Hid::GetTouchPos().px) + "; y=" + std::to_string(Hid::GetTouchPos().py));
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "GameArea: X=" + std::to_string(GameIconArea.x));
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "GameSelect: X=" + std::to_string(GameSelect.x));
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "Mem avail: 0x" + Utils::NumToHexStr(Settings::GetMemAvail(), 8));
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "Mem used: 0x" + Utils::NumToHexStr(Settings::GetMemUsed(), 8));
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "Ae Message: " + std::to_string(App::lastAeCmd));
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "Sams Message: " + std::to_string(App::lastSamsCmd));
        Graphics::DrawText(Fonts::FONT_TINY, X, Y+=s, "Last Error: " + std::to_string(lastErr));
        Y = 0;
    }
}

/*
*   Folder mechanics
*/
int GetAvailFolderID() {
	mINI::INIFile file(SD("/ReiNX/deltaSave.ini"));
	mINI::INIStructure save;
	file.read(save);
	for (int i = 0; i < MAX_FOLDERS; i++) {
		if (!save.has("Folder" + std::to_string(i))) return i;
	}
	return -1;
}

void Dashboard::AddFolder() {
	DEBUG(Debug::Type::DASHBOARD, "Adding folder\n");
    //Load save
    mINI::INIFile file(SD("/ReiNX/deltaSave.ini"));
    mINI::INIStructure save;
    file.read(save);
    
    //Get user input for name
    char out[0xC00] = {0};
#ifdef __SWITCH__
    App::LaunchSwkbd(out, lang->GetString(LANG_FOLDERNAME), "", lang->GetString(LANG_SETNAME), lang->GetString(LANG_FOLDER));
#else
	memcpy(out, "Folder", sizeof("Folder"));
#endif
    std::string name = std::string(out);
    
    //Create folder object
	int folderNum = GetAvailFolderID();
	if (folderNum >= 0) {
		GameFolder *folder = new GameFolder(folderNum, name, folderIcon);
		folder->SetOptionsTexture(formTex);
		GameEntries.push_back(folder);
		save["Folder" + std::to_string(folderNum)]["name"] = name;
	}
	else {
		msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_MAX_FOLDERS));
	}
    
    file.write(save);
}

void Dashboard::DeleteFolder(u64 folder) {
	DEBUG(Debug::Type::DASHBOARD, "Deleting folder #%d\n", folder);
    //Get user input for name
    int ind = 0;
    bool setGames = false;
    for(auto &games : GameEntries) {
        if(games->IsFolder() && games->GetTitleID() == folder) {
            //Load save
            mINI::INIFile file(SD("/ReiNX/deltaSave.ini"));
            mINI::INIStructure save;
            file.read(save);
    
            //Transfer games from folder back to dash
            if(static_cast<GameFolder*>(games)->Games.size() > 0){
				for (auto &dashGames : GameEntries) {				 //iterate through dash games to look for common tids 
					u64 tid = dashGames->GetTitleID();
					for (auto &fg : static_cast<GameFolder*>(games)->Games) { //iterate through folder's games
						if (tid == std::get<0>(fg)) {
							dashGames->SetInFolder(false);
						}
					}
				}
                setGames = true;
            }
			static_cast<GameFolder*>(games)->Games.clear();

            //Erase entry
            save.remove("Folder"+std::to_string(folder));            
            file.write(save);
			break;
        }
        ind++;
    }
    
    if(setGames) SetGames();
}

void Dashboard::MoveSelectedGameToFolder(u64 folderID) { //TODO
    mINI::INIFile file(SD("/ReiNX/deltaSave.ini"));
    mINI::INIStructure save;
    file.read(save);
    
    s32 ind = 0;
    for(auto game: GameEntries) {
        if(ind == App::gameSelectInd && game->GetTitleID() != folderID) {
            for(auto entry: GameEntries) {
                if(entry->IsFolder() && !entry->IsInFolder() && entry->GetTitleID() == folderID) {
                    if(static_cast<GameFolder*>(entry)->Games.size() < MAX_GAMES_PER_FOLDER) {
                        static_cast<GameFolder*>(entry)->AddGame(game->GetTitleID(), game->GetIcon(), game->GetName());
                        save["Folder"+std::to_string(folderID)][std::to_string(game->GetTitleID())] = "";
                        file.write(save);
                    }
                    else {
                        msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_FOLDERFULL));
                    }
                }
            }
            return;
        }
        ind++;
    }
    msgBox->Show(lang->GetString(LANG_ERROR), "Folder doesn't exist!");
    
    return;
}

/*
*   Controls
*/
void Dashboard::ActivateDash() {
    Audio::Play(SFX_Activate);
    Result rc = 0;
    
    if(App::dashLayer > 0) {
        
        //Boot game if nothings running
        if(!App::currentApplication.active || GameEntries[App::gameSelectInd]->IsFolder()) {
            rc = GameEntries[App::gameSelectInd]->Run();
        }
        //Close game if you're trying to launch a different game
        else if(App::currentApplication.active && App::currentApplication.tid != GameEntries[App::gameSelectInd]->GetTitleID()) {
            if(msgBox->Show(lang->GetString(LANG_CLOSING), lang->GetString(LANG_CLOSE_APP), MSGBOX_YESNO) == RESULT_YES) {
                App::CloseGame();
                rc = GameEntries[App::gameSelectInd]->Run();
            }
        }
        //If launching same game thats running, unsuspend it
        else {
            rc = App::RequestForegroundApp();
        }
        //If launching folder, setgames
        if(!GameEntries[App::gameSelectInd]->IsFolder()) 
            SetGames();
    }
    if(App::dashLayer == 0) 
        rc = AppletIcons[App::appletSelectInd]->Run();
    
    if(rc) msgBox->Show(lang->GetString(LANG_ERROR), std::to_string(rc));
}

void Dashboard::SelectGame(GameBase *gbase, s32 ind) {
    if (ind == App::gameSelectInd && App::dashLayer > 0 && !draggingGameArea) {
        lastErr = gbase->Run();
        if(lastErr) 
            msgBox->Show(lang->GetString(LANG_ERROR), lang->GetString(LANG_ERROR_CODE) + ": " + std::to_string(lastErr));
        App::RequestForegroundDelta();
    } else {
        App::gameSelectInd = ind;
        App::dashLayer = 1;
    }
}

void Dashboard::CloseApp() {
    if(App::currentApplication.active && App::currentApplication.tid == GameEntries[App::gameSelectInd]->GetTitleID()) {
        if(msgBox->Show(lang->GetString(LANG_CLOSING), lang->GetString(LANG_CLOSE_APP), MSGBOX_YESNO) == RESULT_YES) {
            App::CloseGame();
        }
    }
}

void Dashboard::IncrementDashSel() {
    Audio::Play(SFX_Navigate);
    
    if(App::dashLayer > 0 && App::gameSelectInd < (s32)GameEntries.size()-1) {
        App::gameSelectInd++;
    }
    if(App::dashLayer == 0 && App::appletSelectInd < (s32)AppletIcons.size()-1) 
        App::appletSelectInd++;
}

void Dashboard::DecrementDashSel() {
    Audio::Play(SFX_Navigate);
    
    if(App::dashLayer > 0 && App::gameSelectInd > 0) {
        App::gameSelectInd--;
    }
    if(App::dashLayer == 0 && App::appletSelectInd > 0) 
        App::appletSelectInd--;
}

void Dashboard::IncrementLayer() {
    if(App::dashLayer < gameRows) App::dashLayer++;
    Audio::Play(SFX_Navigate);
}

void Dashboard::DecrementLayer() {
    if(App::dashLayer > 0) App::dashLayer--;
    Audio::Play(SFX_Navigate);
}

Result Dashboard::OpenSettings() {
	DEBUG(Debug::Type::DASHBOARD, "Opening settings\n");
	settings->Initialize();
    settings->Show();
    return 0;
}

Result Dashboard::OpenNews() {
	DEBUG(Debug::Type::DASHBOARD, "Opening news\n");
	news->Initialize();
    news->Show();
    return 0;
}

void Dashboard::UpdateMenus() {
    if(news) news->Update();
    if(settings) settings->Update();
}

void Dashboard::OffsetGameIcons(u32 deltaX) {
    GameIconArea.x += deltaX;
    if(GameIconArea.x > 120) GameIconArea.x = 120;
}