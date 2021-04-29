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

#define STB_IMAGE_IMPLEMENTATION
#include "GL/stb_image.h"
#include "Graphics.hpp"

GLFWwindow *GL::Window;
RGBA Graphics::selectCol;
RGBA Graphics::msgCol;
RGBA Graphics::butCol;
RGBA Graphics::headerCol;
Rect Graphics::Screen;
Context Graphics::ctxt;
GLuint GL::shaderProg;

void Graphics::Init(Rect pos) {
    //Basic SDL init
	DEBUG(Debug::Type::GFX, "Init.\n");
    Screen = pos;
    GL::Init(Screen);
    
    //Foreground
#ifdef __SWITCH__
    appletRequestToGetForeground();
    appletSetHandlesRequestToDisplay(true);
#endif
}

void Graphics::Exit() {
	DEBUG(Debug::Type::GFX, "Exit.\n");
#ifdef __SWITCH__
    appletSetHandlesRequestToDisplay(false);
#endif
    GL::Quit();
}

void Graphics::SetupShaders(std::string vsh, std::string fsh) {
	GL::AddShader(vsh, GL_VERTEX_SHADER);
	GL::AddShader(fsh, GL_FRAGMENT_SHADER);
	GL::LinkShaderProgram();
}

void Graphics::DestroyShaders() {
	GL::DestroyShaders();
}

void Graphics::Rectangle(Rect pos, RGBA scolor) {
    GL::Rectangle(pos, scolor.R, scolor.G, scolor.B, scolor.A);
}

Texture *Graphics::CreateTexFromString(std::string file) {
	DEBUG(Debug::Type::GFX, "CreateTexFromString(): %s\n", file.c_str());
	int width = 0, height = 0, channels = 0;
    bool useAlpha = true;
    unsigned char *img = stbi_load(file.c_str(), &width, &height, &channels, useAlpha ? STBI_rgb_alpha : STBI_rgb);
    Texture *tex = nullptr;
	if (!img) return nullptr;
    else {
        tex = GL::SurfToTex(img, width, height, useAlpha ? 4 : 3);
        stbi_image_free(img);
    }
    
    return tex;
}

Texture *Graphics::CreateTexFromBuffer(void *buffer, size_t size) {
	DEBUG(Debug::Type::GFX, "CreateTexFromBuffer()\n");
    Texture *tex = nullptr;
    int width, height, channels;
    unsigned char *img = stbi_load_from_memory((stbi_uc*)buffer, size, &width, &height, &channels, STBI_rgb);
    if(!img) 
		return nullptr;
    else {
        tex = GL::SurfToTex(img, width, height, 3);
        stbi_image_free(img);
    }
    
    return tex;
}

void Graphics::RenderTexture(Texture *tex, Rect pos, Rect *clip, bool useShader) {
	GL::RenderTexture(tex, pos, clip, useShader);
}

void Graphics::DrawImageElem(Texture *tex, Rect pos, ElementType type) {
    if(type & ELEM_Selected){
        Rect p = pos;
        p.x -= 5; p.y -= 5;
        p.w += 10; p.h += 10;
        Rectangle(p, GetSelectCol());
    }
    RenderTexture(tex, pos);
}

void Graphics::DrawText(u8 fntsize, float x, float y, std::string str, RGBA col, u32 wrap) {
    ctxt.color = col;
    ctxt.sizeIndex = fntsize;
    fonts().draw(x, y, str, ctxt);
}

void Graphics::DrawButton(Rect Pos, std::string Text, ElementType butType) {
    int tHight = Fonts::FONT_SIZE_SMALL;
    if(butType & ELEM_DrawOutline) {
        Rect pos; 
        pos.x = Pos.x-5; pos.y = Pos.y-5;
        pos.w = Pos.w+10; pos.h = Pos.h+10;
        Graphics::Rectangle(pos, (butType & ELEM_Selected) ? GetSelectCol() : GetMessageCol());
        Graphics::Rectangle(Pos, GetButtonCol());
    }
    Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + 12, Pos.y + (Pos.h/2) - (tHight/2), Text, (butType & ELEM_Selected) ? GetSelectCol() : GetMessageCol());
}

void Graphics::DrawOption(Rect Pos, std::string Text, std::string OptionText, ElementType type) {
    int tHight = Fonts::FONT_SIZE_SMALL;
    if(type & ELEM_Selected) {
        Rect pos; 
        pos.x = Pos.x-5; pos.y = Pos.y-5;
        pos.w = Pos.w+10; pos.h = Pos.h+10;
        Graphics::Rectangle(pos, GetSelectCol());
        Graphics::Rectangle(Pos, GetButtonCol());
        Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + 10, Pos.y + (Pos.h/2) - (tHight/2), Text);
        Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + ((Text != "") ? 200 : 40), Pos.y + (Pos.h/2) - (tHight/2), OptionText, GetSelectCol());
    }
    else {
        Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + 10, Pos.y + (Pos.h/2) - (tHight/2), Text);
        Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + ((Text != "") ? 200 : 40), Pos.y + (Pos.h/2) - (tHight/2), OptionText);
    }
}

Texture *Graphics::BlitSurfacesFromString(std::vector<std::string> layers, Rect pos) {
	DEBUG(Debug::Type::GFX, "BlitSurfacesFromString()\n");
    int width = 0, height = 0, channels = 0, i = 0;
    Texture *tex = nullptr;
	unsigned char *img = nullptr;
	do {
		DEBUG(Debug::Type::GFX, "Blitting surface %s\n", layers[i].c_str());
		if (!img) {
			img = stbi_load(layers[i++].c_str(), &width, &height, &channels, STBI_rgb_alpha);
		}
		else {
			unsigned char *lay = stbi_load(layers[i++].c_str(), &width, &height, &channels, STBI_rgb_alpha);
			GL::Blit(img, lay, width * height * 4);
			stbi_image_free(lay);
		}
	} while (i < layers.size());

    tex = GL::SurfToTex(img, width, height, 4);
    stbi_image_free(img);
    return tex;
}

void Graphics::FreeTexture(Texture *tex) {
	DEBUG(Debug::Type::GFX, "FreeTexture()\n");
    GL::FreeTexture(tex);
}

void Graphics::Render() {
    GL::Render();
}

void Graphics::Clear() {
    GL::Clear();
}