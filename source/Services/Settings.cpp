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

#include "Settings.hpp"

SelectType Settings::gameSelType = SELECT_OUTLINE;
GameAreaType Settings::gameAreaType = GAMEAREA_FULL;
std::string Settings::currentTheme;
bool Settings::LockIconEnabled;
bool Settings::AddFolder = false;
s32 Settings::deleteFolder = -1;
bool Settings::addToFolder = false;
bool Settings::reloadTheme = false;
std::vector<std::string> Settings::Langs = {"JA", "EN-US", "FR", "DE", "IT", "ES", "ZH-CN", "KO", "NL", "PT", "RU", "ZH-TW", "EN-GB", "FR-CA", "ES419", "ZH-HANS", "ZH-HANT"};
std::vector<std::string> Settings::Regions = {"Japan", "USA", "Europe", "Australia", "China", "Korea", "Taiwan"};

std::string Settings::GetFirmwareVersion() {
	SetSysFirmwareVersion firm;
    setsysGetFirmwareVersion(&firm);
    std::string str = std::string(firm.display_version);
    
    return str;
}

std::string Settings::GetSerialNumber() {
	SetSysSerialNumber serial;
	setsysGetSerialNumber(&serial);
    std::string str = std::string(serial.number);
	return str;
}

std::string Settings::GetDeviceNickname() {
    SetSysDeviceNickName nick = {};
    setsysGetDeviceNickname(&nick);
    std::string str = std::string(nick.nickname);
    return str;
}

u64 Settings::GetSystemLangCode() {
    Result rc = 0;
    u64 code = 0;
    rc = setGetSystemLanguage(&code);
    return R_SUCCEEDED(rc) ? code : 0;
}

void Settings::SetDeviceNickname(std::string nick) {
    SetSysDeviceNickName nickname = {};
    snprintf(nickname.nickname, sizeof(nickname.nickname), "%s", nick.c_str());
    setsysSetDeviceNickname(&nickname);
}

bool Settings::GetLockScreenFlag() {
    bool flag = false;
    setsysGetLockScreenFlag(&flag);
    return flag;
}

void Settings::SetLockScreenFlag(bool flag) {
    setsysSetLockScreenFlag(flag);
}

std::string Settings::GetThemePath() {
    return currentTheme.empty() ? "" : std::string(SD("/Themes/")) + currentTheme; 
}

size_t Settings::GetMemAvail() {
    size_t mem_available = 0;
    svcGetInfo(&mem_available, 6, CUR_PROCESS_HANDLE, 0);
    return mem_available;
}

size_t Settings::GetMemUsed() {
    size_t mem_used = 0;
    svcGetInfo(&mem_used, 7, CUR_PROCESS_HANDLE, 0);
    return mem_used;
}

SetLanguage Settings::GetLangIndex() {
    SetLanguage code = SetLanguage_ENUS;
    u64 c;
    setGetSystemLanguage(&c);
    setMakeLanguage(c, &code);
    return code;
}

SetRegion Settings::GetRegionIndex() {
    SetRegion code = SetRegion_USA;
    setGetRegionCode(&code);
    return code;
}

bool Settings::HasAutoRCM() {
	bool ret = false;
	FsStorage store;
	
	Result rc =  fsOpenBisStorage(&store, FsBisPartitionId_BootPartition1Root);
    if (R_FAILED(rc)) return false;
	s64 size = 0;
	fsStorageGetSize(&store, &size);
	if(size==0) return -1;
	char * buf = (char *)malloc(size);
	rc = fsStorageRead(&store, 0, buf, size);
    
	//this loop xors 0x77 with the first byte of the first four BCTs
	for(int i=0; i < 4; i++){
		int off = 0x210 + (i*0x4000);
		ret = !(buf[off] == 0xF7 || buf[off] == 0x37); //check both prod and dev
	}
    
	fsStorageClose(&store);
	free(buf);
	return ret;
}

Result Settings::ToggleAutoRCM() {
    Result rc = 0;
	FsStorage store;
	rc =  fsOpenBisStorage(&store, FsBisPartitionId_BootPartition1Root);
    if (R_FAILED(rc)) return rc;
	s64 size = 0;
	fsStorageGetSize(&store, &size);
	if(size==0) return -1;
	char * buf = (char *)malloc(size);
	rc = fsStorageRead(&store, 0, buf, size);
    
	//this loop xors 0x77 with the first byte of the first four BCTs
	for(int i=0; i < 4; i++){
		int off = 0x210 + (i*0x4000);
		buf[off] ^= 0x77;
	}

	//write the modified boot0 back to the emmc
	rc = fsStorageWrite(&store, 0, buf, size);
	fsStorageClose(&store);
	free(buf);
    return rc;
}

void Settings::SaveCurrTheme() {
	DEBUG(Debug::Type::SETTINGS, "Save current theme\n");
    mINI::INIFile file(SD("/ReiNX/deltaSave.ini"));
    mINI::INIStructure ini;
    ini["SaveData"]["theme"] = currentTheme;
    file.write(ini);
}

void Settings::LoadSavedTheme() {
	DEBUG(Debug::Type::SETTINGS, "Loading saved theme\n");
    mINI::INIFile file(SD("/ReiNX/deltaSave.ini"));
    mINI::INIStructure ini;
    file.read(ini);
    currentTheme = ini["SaveData"]["theme"];
}