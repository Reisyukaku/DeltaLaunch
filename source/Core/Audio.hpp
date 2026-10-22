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

enum SfxType {
    SFX_Bgm,
    SFX_Navigate,
    SFX_Activate,
    SFX_Back,
    SFX_Error
};

class Audio {
public:
    static void Init(float volume);
    static void Exit();
    static void SetSfx(std::string file, SfxType type);
    static void Play(SfxType type, bool loop = false, int ms = 0);
    static void Stop(int ms = 0);
    static void Pause();
    static void Resume();
};
