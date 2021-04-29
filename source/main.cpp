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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "Core/types.h"
#include "Core/types.h"

#include "Core/Engine.hpp"

#ifdef __SWITCH__
extern "C"{
    u32 __nx_applet_type = AppletType_SystemApplet;
    size_t __nx_heap_size = 0xD000000;
    
    void userAppInit(void);
    void userAppExit(void);
}

void userAppInit(void) {
    nsInitialize();
    plInitialize(PlServiceType_System);
    setInitialize();
    setsysInitialize();
    accountInitialize(AccountServiceType_System);
    Rnx::Initialize();
}

void userAppExit(void) {
    nsExit();
    plExit();
    setExit();
    setsysExit();
    accountExit();
    Rnx::Exit();
}
#endif

int main(int argc, char* argv[]) {
    //Qlaunch loop
    Engine eng(1280, 720);
    eng.Initialize();
    while (eng.running) {
        eng.Clear();
        eng.Update();
        eng.Render();
    }
    
    return 0;
}