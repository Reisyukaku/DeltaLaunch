#pragma once

#include <unordered_map>
#include <vector>
#include <map>
#include "types.h"
#include "GL/GL.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef __SWITCH__
extern "C" {
#include <switch/services/pl.h>
}
#else
#define PlSharedFontType_Total 5
struct PlFontData
{
	u32 type;
	u32 offset;
	u32 size;
	void* address;
};
#endif

#define TOTAL_FONT_FACES 0x10

#define FON_A u8"\uE0E0"
#define FON_B u8"\uE0E1"
#define FON_X u8"\uE0E2"
#define FON_Y u8"\uE0E3"
#define FON_L u8"\uE0E4"
#define FON_R u8"\uE0E5"
#define FON_ZL u8"\uE0E6"
#define FON_ZR u8"\uE0E7"
#define FON_SL u8"\uE0E8"
#define FON_SR u8"\uE0E9"
#define FON_UP u8"\uE0EB"
#define FON_DN u8"\uE0EC"
#define FON_LT u8"\uE0ED"
#define FON_RT u8"\uE0EE"
#define FON_PL u8"\uE0EF"
#define FON_MI u8"\uE0F0"
#define FON_HM u8"\uE0F4"
#define FON_SS u8"\uE0F5"
#define FON_LS u8"\uE101"
#define FON_RS u8"\uE102"
#define FON_L3 u8"\uE104"
#define FON_R3 u8"\uE105"

#define FON_SPINNER_0 u8"\uE020"
#define FON_SPINNER_1 u8"\uE021"
#define FON_SPINNER_2 u8"\uE022"
#define FON_SPINNER_3 u8"\uE023"
#define FON_SPINNER_4 u8"\uE024"
#define FON_SPINNER_5 u8"\uE025"
#define FON_SPINNER_6 u8"\uE026"
#define FON_SPINNER_7 u8"\uE027"

#define FON_CHAT u8"\uE132"
#define FON_PC u8"\uE139"
#define FON_SD u8"\uE121"
#define FON_GAME u8"\uE12C"
#define FON_ERROR u8"\uE150"
#define FON_TICKET2 u8"\uE133"
#define FON_SHOP u8"\uE133"
#define FON_JOYCON u8"\uE136"
#define FON_POWER u8"\uE0B8"

#define FON_REFRESH u8"\uE08E"

#define FON_BACKSPACE	u8"\uE070"
#define FON_CHECK       u8"\ue200"
#define FON_NEXT        u8"\ue201"
#define FON_MUSIC       u8"\ue202"
#define FON_PREVIOUS    u8"\ue203"
#define FON_SHUFFLE     u8"\ue204"
#define FON_REWIND      u8"\ue205"
#define FON_TRASH       u8"\ue206"
#define FON_WRENCH      u8"\ue207"
#define FON_WIDGET      u8"\ue208"
#define FON_PAUSE       u8"\ue209"
#define FON_PLAY        u8"\ue20a"
#define FON_STOP        u8"\ue20b"
#define FON_BATTERY_0   u8"\ue20c"
#define FON_BATTERY_1   u8"\ue20d"
#define FON_BATTERY_2   u8"\ue20e"
#define FON_BATTERY_3   u8"\ue20f"
#define FON_BATTERY_4   u8"\ue210"
#define FON_BATTERY_POWER       u8"\ue211"
#define FON_WIFI_1      u8"\ue212"
#define FON_WIFI_2      u8"\ue213"
#define FON_WIFI_3      u8"\ue214"
#define FON_FAST_FORWARD	u8"\ue215"
#define FON_EJECT       u8"\ue217"
#define FON_KEY			u8"\ue218"
#define FON_MAGNIFYING_GLASS    u8"\ue219"
#define FON_DISLIKE     u8"\ue21a"
#define FON_LIKE        u8"\ue21b"
#define FON_BURST_NEW   u8"\ue21c"
#define FON_BURST       u8"\ue21e"
#define FON_SKULL       u8"\ue21f"
#define FON_PAGE_MULTIPLE       u8"\ue220"
#define FON_FILE_ZIP    u8"\ue221"
#define FON_FILE_AUDIO  u8"\ue222"
#define FON_FILE_CODE   u8"\ue223"
#define FON_FILE_EXCEL  u8"\ue224"
#define FON_FILE_IMAGE  u8"\ue225"
#define FON_FILE        u8"\ue226"
#define FON_FILE_PDF    u8"\ue227"
#define FON_FILE_POWERPOINT     u8"\ue228"
#define FON_FILE_TEXT   u8"\ue229"
#define FON_FILE_TEXT_1 u8"\ue22a"
#define FON_FILE_VIDEO  u8"\ue22b"
#define FON_FILE_WORD   u8"\ue22c"
#define FON_SITEMAP     u8"\ue22d"
#define FON_FOLDER      u8"\ue22e"
#define FON_TICKET      u8"\ue22f"
#define FON_INFO_CIRCLE u8"\ue230"
#define FON_HDD			u8"\ue231"
#define FON_WEB			u8"\ue233"
#define FON_TREE        u8"\ue234"
#define FON_NAND        u8"\ue235"
#define FON_USB			u8"\ue236"
#define FON_SD_CARD		u8"\ue237"

#define MAX_TEXTURE_WIDTH 8192
#define MAX_FONT_AR 0.6f
#define MAX_ORDINAL 0x00FFFFFF

#define FONT_PAGE_SIDE 0x10
#define FONT_PAGE_SIZE 0x100
#define MAX_CODEPOINT 0xE300

struct Context
{
	Context() {}
	Context(const RGBA& color, u8 sizeIndex, u16 fontIndex = 4) {}

	FT_Face font = NULL;
	Rect r = {};
	int x = 0;
	int y = 0;
	u8 sizeIndex = 3;
	u8 fontIndex = 4;
	RGBA color;
};

namespace Font {
	enum Type : u8
	{
		Standard = 0x00,
		Chinese_Simplified = 0x01,
		Chinese_Simplified_Extended = 0x02,
		Chinese_Traditional = 0x03,
		Korean = 0x04,
		Extended = 0x05,
		HINDI = 0xB,
		HEBREW = 0xC,
		ARABIC = 0xD,
		FIXED = 0xE,
		SYMBOLS1 = 0xF
	};

	class Cache;

	class Glyph {
	public:
		Glyph();
		virtual ~Glyph();

		const float & width() const { return m_width; }
		const float & height() const { return m_height; }

		float & width() { return m_width; }
		float & height() { return m_height; }


		const float & x() const { return m_x; }
		const float & y() const { return m_y; }

		float& x() { return m_x; }
		float & y() { return m_y; }

		float & offsetX() { return m_offsetX; }
		float & offsetY() { return m_offsetY; }

		int& advance() { return m_advance; }

		bool isValid() const;
	protected:
		float m_width = 0;
		float m_height = 0;
		float m_x = 0;
		float m_y = 0;

		float m_offsetX = 0;
		float m_offsetY = 0;

		int m_advance = 0;
	};

	class Page {
	public:
		Page();
		virtual ~Page();
		void load(Context& settings, u32 ordinal);
		void draw(Cache* cache, Context& settings, const u32 ordinal, u8 fontIndex);

		bool isValid() const;

		Glyph& glyph(u32 ordinal);

		bool close();
	protected:
		void load(Context& settings, u32 offset, u32 ordinal, u8 *tmpSurface);
		Glyph m_glyphs[FONT_PAGE_SIZE];
		Texture *m_texture = nullptr;
		bool m_isLoaded = false;
	};

	class Cache {
	public:
		Cache();
		Cache(FT_Face font);
		virtual ~Cache();

		u32 utf8char(char*& s, const char* max);

		void drawWord(Context& context, char*& ptr, const char* end, u8 fontIndex);
		u64 sizeWord(Context& context, char* ptr, const char* end);

		Page& page(u32 ordinal);

		void draw(float x, float y, const std::string& s, u8 fontIndex);

		Rect rect(const std::string& s, const Context& context);

		int getFontIndex(u32 c, u8 fontIndex);

		FT_Face font(u32 c, u8 fontIndex);
		RGBA& color() { return m_color; }
		u8& sizeIndex() { return m_sizeIndex; }
	private:
		Page m_pages[MAX_CODEPOINT / FONT_PAGE_SIZE];
		FT_Face m_font;
		RGBA m_color;
		u8 m_sizeIndex = 0;
	};
}

class Fonts {
public:
	const static u8 FONT_SIZE_HUGE = 48;
	const static u8 FONT_SIZE_LARGE = 30;
	const static u8 FONT_SIZE_MEDIUM = 26;
	const static u8 FONT_SIZE_SMALL = 22;
	const static u8 FONT_SIZE_TINY = 14;
	const static u8 FONT_SIZE_MICRO = 10;

	const static u8 FONT_HUGE = 5;
	const static u8 FONT_LARGE = 4;
	const static u8 FONT_MEDIUM = 3;
	const static u8 FONT_SMALL = 2;
	const static u8 FONT_TINY = 1;
	const static u8 FONT_MICRO = 0;

	enum Align : u8
	{
		LEFT = 1,
		RIGHT = 2,
		CENTER = 3,

		TOP = 4,
		BOTTOM = 8,
		MIDDLE = 12,
		CENTER_MIDDLE = 12 | 3
	};

	Fonts() : m_caches()
	{
		languageCode = 0;
#ifdef __SWITCH__
		totalFontFaces = 0;
#else
		totalFontFaces = 5;
#endif

		//todo memset(&m_fonts, 0, sizeof(m_fonts));
		memset(&m_fontMap, 0, sizeof(m_fontMap));

#ifdef __SWITCH__
		setGetSystemLanguage(&languageCode);

		if (plGetSharedFont(languageCode, m_fontData, PlSharedFontType_Total, &totalFontFaces))
		{
			return;
		}
#endif
		int i = 0;
		for (; i < (int)totalFontFaces; i++)
		{
			m_fontMap[m_fontData[i].type] = i;
		}

		for (; i < TOTAL_FONT_FACES; i++)
		{
			m_fontMap[i] = i;
		}
	}

	~Fonts() {}

	Font::Cache& fontCache(const Context& settings) {
		u64 key = 0;
		key |= (u64)settings.sizeIndex << 32;
		key |= (u64)settings.fontIndex << 48;

		auto &c = m_caches[key];
		c.color() = settings.color;
		c.sizeIndex() = settings.sizeIndex;

		return c;
	}

	Rect rect(const std::string& s, const Context& settings) {
		auto& cache = fontCache(settings);
		return cache.rect(s, settings);
	}

	void draw(int x, int y, const std::string& s, const Context& settings) {
		if (settings.fontIndex >= TOTAL_FONT_FACES || settings.sizeIndex >= 6)
		{
			ERROR(Debug::Type::FONT, "Font size index too large\n");
			return;
		}

		auto& cache = fontCache(settings);
		cache.draw(x, y, s, settings.fontIndex);
	}

	FT_Face open(u32 languageCode, int sizeIndex);

	static const u8 size(u16 i) {
		switch (i)
		{
			case 0: return FONT_SIZE_MICRO;
			case 1: return FONT_SIZE_TINY;
			case 2: return FONT_SIZE_SMALL;
			case 3: return FONT_SIZE_MEDIUM;
			case 4: return FONT_SIZE_LARGE;
			case 5: return FONT_SIZE_HUGE;
		}
		return 0;
	}

private:
	u8 m_fontMap[TOTAL_FONT_FACES];
	FT_Face m_fonts[TOTAL_FONT_FACES][6];
	PlFontData m_fontData[TOTAL_FONT_FACES];
	std::unordered_map<u64, Font::Cache> m_caches;
	s32 totalFontFaces;
	u64 languageCode;
};

Fonts& fonts();