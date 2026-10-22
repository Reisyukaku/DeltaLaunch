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

#include "Audio.hpp"
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>

namespace {
Mix_Music *bgm = nullptr;
Mix_Music *navSound = nullptr;
Mix_Music *activate = nullptr;
Mix_Music *back = nullptr;
Mix_Music *error = nullptr;
}

void Audio::Init(float vol) {
			SDL_AudioInit(NULL);
            Mix_Init(MIX_INIT_MP3);
            Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
            Mix_VolumeMusic(vol);
        }

void Audio::Exit() {
            Mix_CloseAudio();
            Mix_Quit();
			SDL_AudioQuit();
        }

void Audio::SetSfx(std::string file, SfxType type) {
            if (file == "") return;
            switch(type) {
                case SFX_Bgm:
                {
                    if(bgm) Mix_FreeMusic(bgm);
                    bgm = Mix_LoadMUS(file.c_str());
                    break;
                }
                case SFX_Navigate:
                {
                    if(navSound) Mix_FreeMusic(navSound);
                    navSound = Mix_LoadMUS(file.c_str());
                    break;
                }
                case SFX_Activate:
                {
                    if(activate) Mix_FreeMusic(activate);
                    activate = Mix_LoadMUS(file.c_str());
                    break;
                }
                case SFX_Back:
                {
                    if(back) Mix_FreeMusic(back);
                    back = Mix_LoadMUS(file.c_str());
                    break;
                }
                case SFX_Error:
                {
                    if(error) Mix_FreeMusic(error);
                    error = Mix_LoadMUS(file.c_str());
                    break;
                }
            }
        }

void Audio::Play(SfxType fx, bool loop, int ms) {
            switch(fx) {
                case SFX_Bgm: 
                {
                    if(bgm) Mix_FadeInMusic(bgm, loop ? -1 : 1, ms);
                    break;
                }
                case SFX_Navigate: 
                {
                    if(navSound) Mix_FadeInMusic(navSound, loop ? -1 : 1, ms);
                    break;
                }
                case SFX_Activate: 
                {
                    if(activate) Mix_FadeInMusic(activate, loop ? -1 : 1, ms);
                    break;
                }
                case SFX_Back:
                {
                    if(back) Mix_FadeInMusic(back, loop ? -1 : 1, ms);
                    break;
                }
                case SFX_Error:
                {
                    if(error) Mix_FadeInMusic(error, loop ? -1 : 1, ms);
                    break;
                }
            }
        }

void Audio::Stop(int ms) {
            Mix_FadeOutMusic(ms);
        }

void Audio::Pause() {
            Mix_PauseMusic();
        }

void Audio::Resume() {
            Mix_ResumeMusic();
        }
