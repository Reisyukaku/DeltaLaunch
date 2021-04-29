#include <string.h>
#include <malloc.h>
#include "Font.hpp"

Fonts g_fonts;
FT_Library* g_library = NULL;

FT_Library& library() {
	if (!g_library) {
		g_library = new FT_Library();
		FT_Init_FreeType(g_library);
	}
	return *g_library;
}

Fonts& fonts() { return g_fonts; }

FT_Face TTF_OpenFont(const char* file) {
	FT_Face face;
	if (FT_New_Face(library(), file, 0, &face))
	{
		ERROR(Debug::Type::FONT, "Unable to open font file!\n");
		return NULL;
	}
	return face;
}

FT_Face TTF_OpenFontRW(const u8* buf, u64 sz) {
	FT_Face face;
	if (FT_New_Memory_Face(library(), buf, (FT_Long)sz, 0, &face))
	{
		ERROR(Debug::Type::FONT, "Unable to open font buffer!\n");
		return NULL;
	}
	return face;
}

FT_Face Fonts::open(u32 languageCode, int sizeIndex) {
	DEBUG(Debug::Type::FONT, "Opening font #%d, sizeIndex = %d\n", languageCode, (int)size(sizeIndex));
	if (m_fonts[languageCode][sizeIndex])
	{
		return m_fonts[languageCode][sizeIndex];
	}
	auto fi = m_fontMap[languageCode];

	/*if (languageCode == Font::SYMBOLS1) m_fonts[languageCode][sizeIndex] = TTF_OpenFont(RESOURCES("/fonts/sym1.ttf").c_str());
	else if (languageCode == Font::FIXED) m_fonts[languageCode][sizeIndex] = TTF_OpenFont(RESOURCES("/fonts/fixed.ttf").c_str());
	else if (languageCode == Font::ARABIC) m_fonts[languageCode][sizeIndex] = TTF_OpenFont(RESOURCES("/fonts/tajawal.ttf").c_str());
	else if (languageCode == Font::HEBREW) m_fonts[languageCode][sizeIndex] = TTF_OpenFont(RESOURCES("/fonts/frankruhlhofshi.ttf").c_str());
	else if (languageCode == Font::HINDI) m_fonts[languageCode][sizeIndex] = TTF_OpenFont(RESOURCES("/fonts/hind.ttf").c_str());
	else*/
	{
#ifdef __SWITCH__
		m_fonts[languageCode][sizeIndex] = TTF_OpenFontRW((u8*)m_fontData[fi].address, m_fontData[fi].size);
#else
		switch (languageCode)
		{
		case Font::Extended: m_fonts[languageCode][sizeIndex] = TTF_OpenFont("./Fonts/FontNintendoExtended.ttf"); break;
		case Font::Chinese_Simplified: m_fonts[languageCode][sizeIndex] = TTF_OpenFont("./Fonts/FontChineseSimplified.ttf"); break;
		case Font::Chinese_Traditional: m_fonts[languageCode][sizeIndex] = TTF_OpenFont("./Fonts/FontChineseTraditional.ttf"); break;
		case Font::Chinese_Simplified_Extended: m_fonts[languageCode][sizeIndex] = TTF_OpenFont("./Fonts/FontExtendedChineseSimplified.ttf"); break;
		case Font::Korean: m_fonts[languageCode][sizeIndex] = TTF_OpenFont("./Fonts/FontChineseSimplified.ttf"); break;
		default: m_fonts[languageCode][sizeIndex] = TTF_OpenFont("./Fonts/FontStandard.ttf");
		}
#endif
	}
	return m_fonts[languageCode][sizeIndex];
}


namespace Font
{

	Glyph::Glyph() {}
	Glyph::~Glyph() {}

	bool Glyph::isValid() const {
		return m_width && m_height;
	}

	Page::Page() {}
	Page::~Page() {}

	constexpr u32 pageId(const u32 ordinal) {
		return ordinal / FONT_PAGE_SIZE;
	}

	constexpr u32 pageStart(const u32 ordinal) {
		return (ordinal / FONT_PAGE_SIZE) * FONT_PAGE_SIZE;
	}

	void Page::load(Context& settings, u32 ordinal) {
		if (isValid()) return;
		
		m_isLoaded = true;
		u32 start = pageStart(ordinal);
		u32 end = start + FONT_PAGE_SIZE;

		u32 widthHeight = FONT_PAGE_SIDE * Fonts::size(settings.sizeIndex);
		size_t bufsize = (widthHeight * widthHeight);
		u8 *img = new u8[bufsize]{0};

		for (u32 i = start; i < end; i++) {
			load(settings, i - start, i, img);
		}

		m_texture = GL::SurfToTex(img, widthHeight, widthHeight, 1);

		delete[] img;
	}

	void Page::load(Context& settings, u32 offset, u32 ordinal, u8 *tmpSurface) {
		if (!settings.font) return;

		FT_Select_Charmap(settings.font, FT_ENCODING_UNICODE);
		FT_Set_Pixel_Sizes(settings.font, 0, Fonts::size(settings.sizeIndex));

		if (FT_Load_Char(settings.font, ordinal, FT_LOAD_RENDER)) return;

		const int fontSize = Fonts::size(settings.sizeIndex);
		const int xoffset = ((offset % FONT_PAGE_SIDE) * fontSize);
		const int yoffset = ((offset / FONT_PAGE_SIDE) * fontSize);

		if (settings.font->glyph->bitmap.pixel_mode == 2)
		{
			for (int y = 0; y < (int)settings.font->glyph->bitmap.rows; y++)
			{
				for (int x = 0; x < (int)settings.font->glyph->bitmap.width; x++)
				{
					u32 xs = x + xoffset;
					u32 ys = y + yoffset;
					u32 i = (ys * (FONT_PAGE_SIDE * Fonts::size(settings.sizeIndex)) + xs) * 1;
					
					if (i >= (u32)((FONT_PAGE_SIDE * Fonts::size(settings.sizeIndex)) * (FONT_PAGE_SIDE * Fonts::size(settings.sizeIndex)))) continue;
					tmpSurface[i] = settings.font->glyph->bitmap.buffer[y * settings.font->glyph->bitmap.width + x];
				}
			}

			auto& g = m_glyphs[offset];
			g.x() = xoffset;
			g.y() = yoffset;
			g.width() = settings.font->glyph->bitmap.width;
			g.height() = settings.font->glyph->bitmap.rows;
			g.offsetX() = settings.font->glyph->bitmap_left;
			g.offsetY() = fontSize - (settings.font->glyph->metrics.horiBearingY / 64);
			g.advance() = settings.font->glyph->metrics.horiAdvance / 64;
		}
	}

	bool Page::close() {
		if (m_texture)
			GL::FreeTexture(m_texture);

		return true;
	}

	bool Page::isValid() const {
		return m_isLoaded;
	}

	Glyph& Page::glyph(u32 ordinal) {
		return m_glyphs[ordinal % FONT_PAGE_SIZE];
	}

	void Page::draw(Cache* cache, Context& settings, const u32 ordinal, u8 fontIndex) {
		if (!isValid()) {
			settings.font = cache->font(ordinal, fontIndex);
			settings.sizeIndex = (u32)cache->sizeIndex();
			load(settings, ordinal);
			m_isLoaded = true;
		}

		if (settings.r.w && settings.x > settings.r.w) return;

		auto& g = glyph(ordinal);

		if (!m_texture) return;

		Rect pos = { settings.r.x + settings.x + g.offsetX(), settings.r.y + settings.y + g.offsetY(), g.width(), g.height() };
		glBindTexture(GL_TEXTURE_2D, m_texture->id);
		glEnable(GL_TEXTURE_2D);

		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

		GLfloat minu = GLfloat(g.x()) / (GLfloat)(m_texture->w);
		GLfloat maxu = (GLfloat(g.x()) + GLfloat(pos.w)) / (GLfloat)(m_texture->w);
		GLfloat minv = GLfloat(g.y()) / (GLfloat)(m_texture->h);
		GLfloat maxv = (GLfloat(g.y()) + GLfloat(pos.h)) / (GLfloat)(m_texture->h);

		glBegin(GL_QUADS);
			glColor4ub(settings.color.R, settings.color.G, settings.color.B, 255);
			glTexCoord2f(minu, minv); glVertex2i(pos.x, pos.y);
			glTexCoord2f(minu, maxv); glVertex2i(pos.x, pos.y + pos.h);
			glTexCoord2f(maxu, maxv); glVertex2i(pos.x + pos.w, pos.y + pos.h);
			glTexCoord2f(maxu, minv); glVertex2i(pos.x + pos.w, pos.y);
			glColor4ub(255, 255, 255, 255);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		settings.x += g.advance();
	}

	Cache::Cache() : m_font(NULL) {}
	Cache::Cache(FT_Face font) : m_font(font) {}
	Cache::~Cache() {}

	inline u32 utf8codepoint(const u8* u, int l, u64& inc) {
		inc = 1;
		if (l < 1) return -1;

		unsigned char u0 = u[0];
		if (u[0] <= 127) return  u[0];
		if (l < 2) return -1;

		unsigned char u1 = u[1];
		if (u0 >= 192 && u0 <= 223) {
			inc = 2;
			return (u0 - 192) * 64 + (u1 - 128);
		}

		if (u[0] == 0xed && (u[1] & 0xa0) == 0xa0) {
			return -1; //code points, 0xd800 to 0xdfff
		}

		if (l < 3) return -1;

		unsigned char u2 = u[2];
		if (u0 >= 224 && u0 <= 239) {
			inc = 3;
			return (u0 - 224) * 4096 + (u1 - 128) * 64 + (u2 - 128);
		}

		if (l < 4) return -1;

		unsigned char u3 = u[3]; if (u0 >= 240 && u0 <= 247) {
			inc = 4;
			return (u0 - 240) * 262144 + (u1 - 128) * 4096 + (u2 - 128) * 64 + (u3 - 128);
		}
		return -1;
	}

	u32 Cache::utf8char(char*& s, const char* max) {
		u64 inc = 1;
		u32 r = utf8codepoint((u8*)s, max - s, inc);
		s += inc;
		return r;
	}

	Page& Cache::page(u32 ordinal) {
		if (ordinal > MAX_CODEPOINT) ordinal = 0;
		return m_pages[pageId(ordinal)];
	}

	void Cache::draw(float x, float y, const std::string& s, u8 fontIndex) {
		static Context settings;
		settings.x = 0;
		settings.y = 0;
		settings.r = Rect{ x, y,0,0 };
		settings.color = m_color;

		char* ptr = (char*)s.c_str();
		char* end = ptr + s.length();

		while (ptr < end) {
			u32 ordinal = utf8char(ptr, end);
			page(ordinal).draw(this, settings, ordinal, fontIndex);
		}
	}

	bool isSpace(char c) {
		switch (c)
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case '\v':
			case '\f':
				return true;
		}
		return false;
	}

	void Cache::drawWord(Context& context, char*& ptr, const char* end, u8 fontIndex) {
		while (!isSpace(*ptr) && ptr < end) {
			u32 ordinal = utf8char(ptr, end);
			page(ordinal).draw(this, context, ordinal, fontIndex);
		}
	}

	u64 Cache::sizeWord(Context& context, char* ptr, const char* end) {
		u64 width = 0;
		while (!isSpace(*ptr) && ptr < end) {
			u32 ordinal = utf8char(ptr, end);
			width += page(ordinal).glyph(ordinal).width();
		}
		return width;
	}

	Rect Cache::rect(const std::string& s, const Context& context) {
		Rect r = { 0,0,0,0 };

		char* ptr = (char*)s.c_str();
		char* end = ptr + s.length();
		Context settings = context;

		while (ptr < end) {
			u32 ordinal = utf8char(ptr, end);
			auto& p = page(ordinal);
			if (!p.isValid()) {
				settings.font = font(ordinal, context.fontIndex);
				p.load(settings, ordinal);
			}
			Glyph& g = p.glyph(ordinal);

			r.w += g.advance();
		}

		r.h = Fonts::size(context.sizeIndex);

		return r;
	}

	int Cache::getFontIndex(u32 c, u8 fontIndex) {
		if (fontIndex == Font::FIXED) return Font::FIXED;

		if (c < 0x80) return Font::Standard;

		if (c >= 0xE000 && c <= 0xE152) return Font::Extended;

		if (c <= 0xA97F) {
			if (c >= 0x0900 && c <= 0x097F) return Font::HINDI;
			if (c >= 0x1100 && c <= 0x11FF) return Font::Korean;
			if (c >= 0x3130 && c <= 0x318F) return Font::Korean;
			if (c >= 0x4E00 && c <= 0x9FFF) return Font::Chinese_Simplified;
			if (c >= 0xA960 && c <= 0xA97F) return Font::Korean;
		}
		else {
			if (c >= 0xAC00 && c <= 0xD7A3) return Font::Korean;
			if (c >= 0xD480 && c <= 0xD7BF) return Font::HEBREW;
			if (c >= 0xD7B0 && c <= 0xD7FF) return Font::Korean;
			if (c >= 0xD880 && c <= 0xDBBF) return Font::ARABIC;
			if (c >= 0xE200 && c <= 0xE300) return Font::SYMBOLS1;
		}

		return Font::Standard;
	}

	FT_Face Cache::font(u32 c, u8 fontIndex) {
		return fonts().open(getFontIndex(c, fontIndex), sizeIndex());
	}
}