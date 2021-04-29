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

#include "Threads.hpp"

bool Threads::FriendThread() {
	//
	return true;
}

bool Threads::SystemAppletMessage() {
    #ifdef __SWITCH__
	Event epop;
    Result rc = appletGetPopFromGeneralChannelEvent(&epop);
    if(rc == 0)
    {
        eventWait(&epop, UINT64_MAX);
        AppletStorage st;
        rc = appletPopFromGeneralChannel(&st);
        if(rc == 0) {
			s64 size = 0;
			appletStorageGetSize(&st, &size);
			if(size > 0) {
				struct SAMS{
					u8 magic[4];
					u32 version;
					u32 cmdID;
					u8 unk[0x3f4];
				} __attribute__((packed));
				
				SAMS sams = {0};
				appletStorageRead(&st, 0, &sams, size);
				App::lastSamsCmd = sams.cmdID;
				switch(sams.cmdID) {
					case 1:
					{
						//
						break;
					}
					case 2:
					{
						App::CommandHandler(CMD_Home);
						break;
					}
					case 3:
					{
						App::CommandHandler(CMD_Sleep);
						break;
					}
					case 4:
					{
						//
						break;
					}
					case 5:
					{
						App::CommandHandler(CMD_Shutdown);
						break;
					}
					case 6:
					{
						App::CommandHandler(CMD_Reboot);
						break;
					}
					case 7:
					{
						App::CommandHandler(73);
						break;
					}
					case 8:
					{
						App::CommandHandler(8);
						break;
					}
					case 9:
					{
						App::CommandHandler(9);
						break;
					}
					case 10:
					{
						App::CommandHandler(10);
						break;
					}
					case 11:
					{
						App::CommandHandler(91);
						break;
					}
					case 12:
					{
						App::CommandHandler(92);
						break;
					}
					case 13:
					{
						App::CommandHandler(51);
						break;
					}
					case 14:
					{
						App::CommandHandler(52);
						break;
					}
					case 15:
					{
						App::CommandHandler(53);
						break;
					}
					case 16:
					{
						App::CommandHandler(54);
						break;
					}
					case 17:
					{
						App::CommandHandler(55);
						break;
					}
					case 18:
					{
						//
						break;
					}
					case 19:
					{
						//
						break;
					}
					case 20:
					{
						//
						break;
					}
					case 21:
					{
						//
						break;
					}
				}
			}
		}
    }
    eventClose(&epop);
    #endif
    
	return true;
}

bool Threads::AeMessageThread() {
    #ifdef __SWITCH__
	u32 msg = 0;
	appletGetMessage(&msg);
    App::lastAeCmd = msg;
    switch(msg) {
        case 6:
        {
            //
            break;
        }
        case 20:    //HomeButton
        {
            App::CommandHandler(CMD_Home);
            break;
        }
        case 21:
        {
            App::CommandHandler(13);
            break;
        }
        case 22:    //PowerButton
        case 29:
        case 32:
        {
            App::CommandHandler(CMD_Sleep);
            break;
        }
        case 23:
        {
            App::CommandHandler(17);
            break;
        }
        case 25:
        {
            App::CommandHandler(0);
            break;
        }
        case 26:
        {
            App::CommandHandler(CMD_Wakeup);
            break;
        }
        case 27:
        {
            App::CommandHandler(71);
            break;
        }
        case 28:
        {
            App::CommandHandler(70);
            break;
        }
        case 30:
        {
            App::CommandHandler(appletGetOperationMode() ? 44 : 43);
            break;
        }
        case 33:
        {
            App::CommandHandler(72);
            break;
        }
        case 34:
        {
            //App::CommandHandler(appletIsInControllerFirmwareUpdateSection() & 1 ? 64 : 65);
            break;
        }
    }
    #endif
	
	return true;
}