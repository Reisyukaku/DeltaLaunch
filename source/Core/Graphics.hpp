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

#include <stdint.h>
#include <cmath>
#include <string>
#include <vector>
#include "Font.hpp"
#include "GL/GL.hpp"
#include "../Core/types.h"

typedef enum {
    ELEM_Unselected = BIT(0),
    ELEM_Selected = BIT(1),
    ELEM_DrawOutline = BIT(2)
} ElementType;

class Graphics
{
    public:
        static GLFWwindow *Window;
        
        static void Init(Rect pos);
        static void Exit();
		static void SetupShaders(std::string vsh, std::string fsh);
		static void DestroyShaders();
        static void Rectangle(Rect pos, RGBA scolor = 0xFFFFFFFF);
        static Texture *CreateTexFromString(std::string file);
        static Texture *CreateTexFromBuffer(void *buffer, size_t size);
        static void DrawText(u8 fntsize, float x, float y, std::string str, RGBA col = msgCol, u32 wrap = Screen.w);
        static void RenderTexture(Texture *tex, Rect pos, Rect *clip = nullptr, bool useShader = false);
        static void DrawImageElem(Texture *tex, Rect pos, ElementType type);
        static void DrawButton(Rect Pos, std::string Text, ElementType butType);
        static void DrawOption(Rect Pos, std::string Text, std::string OptionText, ElementType type);
        static Texture *BlitSurfacesFromString(std::vector<std::string> layers, Rect pos);
        static u32 GetWinWidth() { return Screen.w; }
        static u32 GetWinHeight() { return Screen.h; }
        static void FreeTexture(Texture *tex);
        static void Render();
        static void Clear();
        
        //colors
        static RGBA GetSelectCol() { return selectCol; }
        static void SetSelectCol(RGBA col) { selectCol = col; }
        static RGBA GetButtonCol() { return butCol; }
        static void SetButtonCol(RGBA col) { butCol = col; }
        static RGBA GetMessageCol() { return msgCol; }
        static void SetMessageCol(RGBA col) { msgCol = col; }
        static RGBA GetHeaderCol() { return headerCol; }
        static void SetHeaderCol(RGBA col) { headerCol = col; }
        
    private:
        static RGBA selectCol, msgCol, butCol, headerCol;
        static Context ctxt;
        static Rect Screen;
};