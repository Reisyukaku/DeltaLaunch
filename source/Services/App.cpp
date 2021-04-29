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

#include "App.hpp"

AppletApplication App::currentApplication;
AppletHolder App::currentApplet;
s32 App::appletSelectInd = 0;
s32 App::gameSelectInd = 0;
u8 App::dashLayer = 0;

u32 App::lastAeCmd = 0;
u32 App::lastSamsCmd = 0;
bool App::sdRemoved = false;
bool App::wakingup = false;
bool App::refreshTheme = false;

/*
*   Application
*/
Result App::LaunchGame(u64 tid, AccountUid userID) {
    Result rc = 0;
    #ifdef __SWITCH__
    AppletStorage aStore;
    
    rc = appletCreateApplication(&currentApplication, tid);
    if(R_FAILED(rc)) return rc;
    
    appletUnlockForeground();
    
    struct InData{
        u32 code;
        u8 unk1;
        u8 pad[3];
        AccountUid id;
        u8 unk2[0x70];
    } __attribute__((packed));
    
    InData indata = {0};
    indata.code = 0xC79497CA;
    indata.unk1 = 1;
    indata.id = userID;
    
    rc = appletCreateStorage(&aStore, sizeof(InData));
    if(R_FAILED(rc)) return rc;

    rc = appletStorageWrite(&aStore, 0, &indata, sizeof(InData));
    if(R_FAILED(rc)) return rc;
    
    rc = appletApplicationPushLaunchParameter(&currentApplication, AppletLaunchParameterKind_PreselectedUser, &aStore);
    if(R_FAILED(rc)) return rc;

    rc = appletApplicationStart(&currentApplication);
    if(R_FAILED(rc)) return rc;
    
    appletStorageClose(&aStore);
    
    appletApplicationRequestForApplicationToGetForeground(&currentApplication);
    //appletHolderJoin(&currentApplication);
    
    currentApplication.tid = tid;
    currentApplication.active = true;
    #endif
    
    return rc;
}

void App::CloseGame() {
#ifdef __SWITCH__
    if(currentApplication.active) {
        //appletHolderRequestExit(&currentApplication);
        appletApplicationClose(&currentApplication);
        currentApplication.active = false;
        currentApplication.tid = 0;
    }
#endif
}

Result App::GetAppRecords(std::vector<NsApplicationRecord> &recs) {
    Result rc = 0;
    s32 size = 0;
	NsApplicationRecord rec[2000];
#ifdef __SWITCH__
    rc = nsListApplicationRecord(rec, 2000, 0, &size);
    for(int i = 0; i < (int)size; i++)
        recs.push_back(rec[i]);
#else
	NsApplicationRecord pcDummyGame;
	pcDummyGame.application_id = 0x0100000000000000;
	recs.push_back(pcDummyGame);
#endif
    
    return rc;
}

NsApplicationControlData App::GetGameControlData(u64 tid, NsApplicationControlSource flag) {
	NsApplicationControlData buffer;
    size_t s = 0;
#ifdef __SWITCH__
    nsGetApplicationControlData(flag, tid, &buffer, 0x20000, &s);
#else
	NacpStruct dummyNacp;
	NacpLanguageEntry dummyLang;
	memcpy(dummyLang.name, "TEST", 5);
	memcpy(dummyLang.author, "Rei", 4);
	dummyNacp.lang[0] = dummyLang;
	//buffer.icon = ;
	buffer.nacp = dummyNacp;
#endif
    return buffer;
}

bool App::IsGamecardInserted() {
    bool b = false;
#ifdef __SWITCH__
    FsDeviceOperator opt;
    fsOpenDeviceOperator(&opt);
    fsDeviceOperatorIsGameCardInserted(&opt, &b);
#endif
    return b;
}

/*
*   Applet
*/
Result App::LaunchAlbum(u8 arg, bool startupSound) {
    Result rc = 0;
#ifdef __SWITCH__
    LibAppletArgs args;
    AppletStorage storeIn;
    
    appletCreateLibraryApplet(&currentApplet, AppletId_photoViewer, LibAppletMode_AllForeground);
    libappletArgsCreate(&args, 0);
    libappletArgsSetPlayStartupSound(&args, startupSound);
    libappletArgsPush(&args, &currentApplet);
    
    u8 stindata = arg;
    rc = appletCreateStorage(&storeIn, sizeof(stindata));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching album", "Error creating storage.", rc);
    }
    
    rc = appletStorageWrite(&storeIn, 0, &stindata, sizeof(stindata));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching album", "Error writing storage.", rc);
    }
    appletHolderPushInData(&currentApplet, &storeIn);
    
    currentApplet.active = true;
    rc = appletHolderStart(&currentApplet);
    appletHolderJoin(&currentApplet);
    
    appletStorageClose(&storeIn);
    appletHolderClose(&currentApplet);
    currentApplet.active = false;
#endif
    
    return rc;
}

AccountUid App::LaunchPSelect() {
    Result rc = 0;
    AccountUid player;
#ifdef __SWITCH__
    AppletStorage storeIn, storeOut;
    LibAppletArgs args;
    
    rc = appletCreateLibraryApplet(&currentApplet, AppletId_playerSelect, LibAppletMode_AllForeground);
    if(R_FAILED(rc)) {
        ShowError("Error launching player select", "Error creating lib applet.", rc);
    }
    
    libappletArgsCreate(&args, 0);
    libappletArgsPush(&args, &currentApplet);
    
    u8 stindata[0xa0] = { 0 };
    
    rc = appletCreateStorage(&storeIn, sizeof(stindata));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching player select", "Error creating storage.", rc);
    }
    
    rc = appletStorageWrite(&storeIn, 0, stindata, sizeof(stindata));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching player select", "Error writing storage.", rc);
    }
    appletHolderPushInData(&currentApplet, &storeIn);
    
    currentApplet.active = true;
    rc = appletHolderStart(&currentApplet);
    appletHolderJoin(&currentApplet);
    if(R_FAILED(rc)) {
        ShowError("Error launching player select", "Error starting applet.", rc);
    }
    else {
        appletHolderPopOutData(&currentApplet, &storeOut);
        u8 buf[0x18] = {0};
        appletStorageRead(&storeOut, 0, buf, 0x18);
        player = *(AccountUid*)&buf[8];
    }
    appletStorageClose(&storeIn);
    appletStorageClose(&storeOut);
    appletHolderClose(&currentApplet);
    currentApplet.active = false;
    
#endif
    
    return player;
}

Result App::LaunchShop() {
    Result rc = 0;
#ifdef __SWITCH__
    LibAppletArgs args;
    AppletStorage storeIn, storeOut;
    
    appletCreateLibraryApplet(&currentApplet, AppletId_shop, LibAppletMode_AllForeground);
    libappletArgsCreate(&args, 1);
    libappletArgsPush(&args, &currentApplet);
    
    rc = appletCreateStorage(&storeIn, 0x2000);
    if(R_FAILED(rc)) {
        ShowError("Error launching eshop", "Error initializing arg storage", rc);
    }

    char indata[0x2000] = {0};
    sprintf (indata, "eshop://%s", "success" );

    rc = appletStorageWrite(&storeIn, 0, indata, 0x2000);
    if(R_FAILED(rc)) {
        ShowError("Error launching eshop", "Error writing arg storage", rc);
    }
    appletHolderPushInData(&currentApplet, &storeIn);
    
    currentApplet.active = true;
    rc = appletHolderStart(&currentApplet);
    appletHolderJoin(&currentApplet);
    if(R_FAILED(rc)) {
        ShowError("Error launching eshop", "Error starting applet.", rc);
    }
    else {
        appletHolderPopOutData(&currentApplet, &storeOut);
        u8 buf[0x1010] = {0};
        appletStorageRead(&storeOut, 0, buf, sizeof(buf));
        //TODO
    }
    appletStorageClose(&storeIn);
    appletStorageClose(&storeOut);
    appletHolderClose(&currentApplet);
    currentApplet.active = false;
#endif
    
    return rc;
}

Result App::LaunchWebsite(std::string url) {
    Result rc = 0;
#ifdef __SWITCH__
    WebCommonConfig config;
    WebCommonReply reply;
    WebExitReason exitReason = WebExitReason_ExitButton;
    rc = webPageCreate(&config, url.c_str());
    if (R_SUCCEEDED(rc)) {
        currentApplet.active = true;
        rc = webConfigShow(&config, &reply);
        if (R_SUCCEEDED(rc))
            rc = webReplyGetExitReason(&reply, &exitReason);
        currentApplet.active = false;
    }
#endif

    return rc;
}

Result App::LaunchSwkbd(char out[0xc00], std::string title, std::string placehold, std::string oktxt, std::string initTxt) {
    Result rc = 0;
#ifdef __SWITCH__
    SwkbdConfig kbd;
    char tempstr[0xc00] = {0};
    rc = swkbdCreate(&kbd, 0);
    if(R_SUCCEEDED(rc)) {
        swkbdConfigMakePresetDefault(&kbd);
        swkbdConfigSetHeaderText(&kbd, title.c_str());
        swkbdConfigSetOkButtonText(&kbd, oktxt.c_str());
        swkbdConfigSetGuideText(&kbd, placehold.c_str());
        swkbdConfigSetInitialText(&kbd, initTxt.c_str());
        //swkbdConfigSetTextCheckCallback(&kbd, validate_text);
        rc = swkbdShow(&kbd, tempstr, sizeof(tempstr));
        if(R_SUCCEEDED(rc)) {
            if(tempstr[0] != '\0')
                strcpy(out, tempstr);
        }else if(rc != (Result)0x5d59) {
            ShowError("Software Keyboard Error", "Unknown error, Lookup errorcode for more info", rc);
        }
    }else{
        ShowError("Software Keyboard Error", "Unknown error, Lookup errorcode for more info", rc);
    }
    swkbdClose(&kbd);
#endif
    return rc;
}

Result App::LaunchNetConnect(u32 arg) {
    Result rc = 0;
    u32 outData = 0;
#ifdef __SWITCH__
    AppletStorage storeIn, storeOut;
    
    appletCreateLibraryApplet(&currentApplet, AppletId_netConnect, LibAppletMode_AllForeground);
    
    rc = appletCreateStorage(&storeIn, sizeof(arg));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching netconnect", "Error creating storage.", rc);
    }
    
    rc = appletStorageWrite(&storeIn, 0, &arg, sizeof(arg));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching netconnect", "Error writing storage.", rc);
    }
    appletHolderPushInData(&currentApplet, &storeIn);
    
    currentApplet.active = true;
    rc = appletHolderStart(&currentApplet);
    appletHolderJoin(&currentApplet);
    
    if(R_FAILED(rc)) {
        ShowError("Error launching netconnect", "Error starting applet.", rc);
    }
    else {
        appletHolderPopOutData(&currentApplet, &storeOut);
        appletStorageRead(&storeOut, 0, &outData, 4);
    }
    
    appletStorageClose(&storeIn);
    appletStorageClose(&storeOut);
    appletHolderClose(&currentApplet);
    
    currentApplet.active = false;
#endif
    return outData;
}

Result App::LaunchController() {
    Result rc = 0;
#ifdef __SWITCH__
    LibAppletArgs args;
    AppletStorage storeIn, storeOut;
    
    appletCreateLibraryApplet(&currentApplet, AppletId_controller, LibAppletMode_AllForeground);
    
    libappletArgsCreate(&args, 0);
    libappletArgsSetPlayStartupSound(&args, 0);
    libappletArgsPush(&args, &currentApplet);
    
    u32 inData1[5] = {0};
    u32 inData2[0x87] = {0};
    inData1[0] = 0x14;
    inData2[0] = 0x21C;
    rc = appletCreateStorage(&storeIn, sizeof(inData1));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching controller", "Error creating storage.", rc);
    }
    
    rc = appletStorageWrite(&storeIn, 0, &inData1, sizeof(inData1));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching controller", "Error writing storage.", rc);
    }
    appletHolderPushInData(&currentApplet, &storeIn);
    appletStorageClose(&storeIn);
    
    rc = appletCreateStorage(&storeIn, sizeof(inData2));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching controller", "Error creating storage.", rc);
    }
    
    rc = appletStorageWrite(&storeIn, 0, &inData2, sizeof(inData2));
    if(R_FAILED(rc)) {
        appletStorageClose(&storeIn);
        ShowError("Error launching controller", "Error writing storage.", rc);
    }
    appletHolderPushInData(&currentApplet, &storeIn);
    appletStorageClose(&storeIn);
    
    currentApplet.active = true;
    rc = appletHolderStart(&currentApplet);
    appletHolderJoin(&currentApplet);
    
    //u8 buf[0xC] = {0};
    if(R_FAILED(rc)) {
        ShowError("Error launching controller", "Error starting applet.", rc);
    }
    else {
        appletHolderPopOutData(&currentApplet, &storeOut);
        u8 buf[0x18] = {0};
        appletStorageRead(&storeOut, 0, buf, 0xC);
    }
    appletStorageClose(&storeOut);
    appletHolderClose(&currentApplet);
    currentApplet.active = false;
#endif
    
    return rc;
}

Result App::ShowError(std::string errText, std::string details, Result rc) {
#ifdef __SWITCH__
    AppletStorage errStor;
    LibAppletArgs args;

    appletCreateLibraryApplet(&currentApplet, AppletId_error, LibAppletMode_AllForeground);
    libappletArgsCreate(&args, 1);
    libappletArgsPush(&args, &currentApplet);
    appletCreateStorage(&errStor, 0x1018);
    u8 argBuf[0x1018] = {0};
    argBuf[0] = 1;
    
    *(u64*)&argBuf[8] = (((rc & 0x1ffu) + 2000) | (((rc >> 9) & 0x1fff & 0x1fffll) << 32));
    strcpy((char*) &argBuf[24], errText.c_str());
    strcpy((char*) &argBuf[0x818], details.c_str());
    appletStorageWrite(&errStor, 0, argBuf, 0x1018);
    appletHolderPushInData(&currentApplet, &errStor);
    
    currentApplet.active = true;
    appletHolderStart(&currentApplet);
    appletHolderJoin(&currentApplet);
    appletHolderClose(&currentApplet);
    currentApplet.active = false;
#endif
    
    return 0;
}

Result App::LaunchHbl() {
    Result rc = 0;
#ifdef __SWITCH__
    appletCreateLibraryApplet(&currentApplet, AppletId_offlineWeb, LibAppletMode_AllForeground);
    currentApplet.active = true;
    rc = appletHolderStart(&currentApplet);
    appletHolderJoin(&currentApplet);
    appletHolderClose(&currentApplet);
    currentApplet.active = false;
#endif
    
    return rc;
}

Result App::RequestForegroundApp() {
	Result rc = 0;
	DEBUG(Debug::Type::APPLET, "RequestForegroundApp\n");
#ifdef __SWITCH__
	rc = appletApplicationRequestForApplicationToGetForeground(&App::currentApplication);
#endif
	return rc;
}

void App::RequestForegroundDelta() {
	DEBUG(Debug::Type::APPLET, "RequestForegroundDelta\n");
#ifdef __SWITCH__
	appletRequestToGetForeground();
#endif
}

/*
*   Misc
*/
Result App::CommandHandler(u32 cmd) {
	DEBUG(Debug::Type::APPLET, "Command %d\n", cmd);
    switch(cmd) {
        case CMD_Home:
        {
            
            if(currentApplet.active) { //applet takes presedence
#ifdef __SWITCH__
                appletHolderRequestExit(&currentApplet);
#endif
            }
            else if(currentApplication.active) {
				RequestForegroundDelta();
            }
            else {
                gameSelectInd = 0;
                appletSelectInd = 0;
                dashLayer = (u8)-1;
            }
            break;
        }
        case 14:
        {
            //
            break;
        }
        case CMD_Sleep:
        {
            Power::Sleep();
            break;
        }
        case CMD_Wakeup:
        {
            App::wakingup = Settings::GetLockScreenFlag() == 1 ? true : false;
            break;
        }
        case CMD_Shutdown:
        {
            Power::Shutdown();
            break;
        }
        case CMD_Reboot:
        {
            Power::Reboot();
            break;
        }
        case CMD_RemovedSD:
        {
            App::sdRemoved = true;
            break;
        }
    }
    
    return 0;
}