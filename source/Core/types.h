#pragma once
#ifdef __SWITCH__
#include <switch.h>
#include "Debug.hpp"
#define RESOURCES(x) (std::string("romfs:") + std::string(x))
#define SD(x) (std::string(x))
#define DEBUG(...)
#define ERROR(...)
#elif __DEBUG__
#define DEBUG(...) (Debug::Write(NULL, __VA_ARGS__));
#define ERROR(...) (Debug::Write(NULL, __VA_ARGS__));
#else
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Debug.hpp"
#undef MessageBox
#undef ERROR

/* Defines */
#define DEBUG(...) (Debug::Write(0x0E, __VA_ARGS__));
#define ERROR(...) (Debug::Write(0x0C, __VA_ARGS__));
#define RESOURCES(x) (std::string("../romfs") + std::string(x))
#define SD(x) (std::string("./SD") + std::string(x))
#define R_SUCCEEDED(res)   ((res)==0)
#define R_FAILED(res)      ((res)!=0)
#define BIT(n) (1U<<(n))

/* Data types */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint64_t u128;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef int64_t s64;

typedef unsigned Result;

/* Enums and Structs */
typedef enum
{
	KEY_A = BIT(0),       ///< A
	KEY_B = BIT(1),       ///< B
	KEY_X = BIT(2),       ///< X
	KEY_Y = BIT(3),       ///< Y
	KEY_LSTICK = BIT(4),       ///< Left Stick Button
	KEY_RSTICK = BIT(5),       ///< Right Stick Button
	KEY_L = BIT(6),       ///< L
	KEY_R = BIT(7),       ///< R
	KEY_ZL = BIT(8),       ///< ZL
	KEY_ZR = BIT(9),       ///< ZR
	KEY_PLUS = BIT(10),      ///< Plus
	KEY_MINUS = BIT(11),      ///< Minus
	KEY_DLEFT = BIT(12),      ///< D-Pad Left
	KEY_DUP = BIT(13),      ///< D-Pad Up
	KEY_DRIGHT = BIT(14),      ///< D-Pad Right
	KEY_DDOWN = BIT(15),      ///< D-Pad Down
	KEY_LSTICK_LEFT = BIT(16),      ///< Left Stick Left
	KEY_LSTICK_UP = BIT(17),      ///< Left Stick Up
	KEY_LSTICK_RIGHT = BIT(18),      ///< Left Stick Right
	KEY_LSTICK_DOWN = BIT(19),      ///< Left Stick Down
	KEY_RSTICK_LEFT = BIT(20),      ///< Right Stick Left
	KEY_RSTICK_UP = BIT(21),      ///< Right Stick Up
	KEY_RSTICK_RIGHT = BIT(22),      ///< Right Stick Right
	KEY_RSTICK_DOWN = BIT(23),      ///< Right Stick Down
	KEY_SL_LEFT = BIT(24),      ///< SL on Left Joy-Con
	KEY_SR_LEFT = BIT(25),      ///< SR on Left Joy-Con
	KEY_SL_RIGHT = BIT(26),      ///< SL on Right Joy-Con
	KEY_SR_RIGHT = BIT(27),      ///< SR on Right Joy-Con

	KEY_HOME = BIT(18),      ///< HOME button, only available for use with HiddbgHdlsState::buttons.
	KEY_CAPTURE = BIT(19),      ///< Capture button, only available for use with HiddbgHdlsState::buttons.

	// Pseudo-key for at least one finger on the touch screen
	KEY_TOUCH = BIT(28),

	// Buttons by orientation (for single Joy-Con), also works with Joy-Con pairs, Pro Controller
	KEY_JOYCON_RIGHT = BIT(0),
	KEY_JOYCON_DOWN = BIT(1),
	KEY_JOYCON_UP = BIT(2),
	KEY_JOYCON_LEFT = BIT(3),

	// Generic catch-all directions, also works for single Joy-Con
	KEY_UP = KEY_DUP | KEY_LSTICK_UP | KEY_RSTICK_UP,    ///< D-Pad Up or Sticks Up
	KEY_DOWN = KEY_DDOWN | KEY_LSTICK_DOWN | KEY_RSTICK_DOWN,  ///< D-Pad Down or Sticks Down
	KEY_LEFT = KEY_DLEFT | KEY_LSTICK_LEFT | KEY_RSTICK_LEFT,  ///< D-Pad Left or Sticks Left
	KEY_RIGHT = KEY_DRIGHT | KEY_LSTICK_RIGHT | KEY_RSTICK_RIGHT, ///< D-Pad Right or Sticks Right
	KEY_SL = KEY_SL_LEFT | KEY_SL_RIGHT,                        ///< SL on Left or Right Joy-Con
	KEY_SR = KEY_SR_LEFT | KEY_SR_RIGHT,                        ///< SR on Left or Right Joy-Con
} HidControllerKeys;

typedef enum {
	SetRegion_JPN = 0, ///< Japan
	SetRegion_USA = 1, ///< The Americas
	SetRegion_EUR = 2, ///< Europe
	SetRegion_AUS = 3, ///< Australia/New Zealand
} SetRegion;

typedef struct {
	u64 application_id;            ///< ApplicationId.
	u8 type;                       ///< Type.
	u8 unk_x09;                    ///< Unknown.
	u8 unk_x0a[6];                 ///< Unknown.
	u8 unk_x10;                    ///< Unknown.
	u8 unk_x11[7];                 ///< Unknown.
} NsApplicationRecord;

typedef struct {
	char name[0x200];
	char author[0x100];
} NacpLanguageEntry;

typedef struct {
	NacpLanguageEntry lang[16];

	u8  x3000_unk[0x24];////Normally all-zero?
	u32 x3024_unk;
	u32 x3028_unk;
	u32 x302C_unk;
	u32 x3030_unk;
	u32 x3034_unk;
	u64 titleID0;

	u8 x3040_unk[0x20];
	char version[0x10];

	u64 titleID_DlcBase;
	u64 titleID1;

	u32 x3080_unk;
	u32 x3084_unk;
	u32 x3088_unk;
	u8 x308C_unk[0x24];//zeros?

	u64 titleID2;
	u64 titleIDs[7];//"Array of application titleIDs, normally the same as the above app-titleIDs. Only set for game-updates?"

	u32 x30F0_unk;
	u32 x30F4_unk;

	u64 titleID3;//"Application titleID. Only set for game-updates?"

	char bcatPassphrase[0x40];
	u8 x3140_unk[0xEC0];//Normally all-zero?
} NacpStruct;

typedef struct {
	NacpStruct nacp;
	u8 icon[0x20000];//JPEG
} NsApplicationControlData;

typedef struct {
	u64 layer_handle;                  ///< Output from GetIndirectLayerConsumerHandle on 2.0.0+.
	bool creating_self;                ///< When set, indicates that the LibraryApplet title is creating itself.
	bool active;
	u64 tid;
} AppletHolder;

typedef struct touchPosition
{
	u32 id;
	u32 px;
	u32 py;
	u32 dx;
	u32 dy;
	u32 angle;
} touchPosition;

typedef struct {
	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	u8 pad;
} TimeCalendarTime;

typedef struct {
	u32 wday; ///< 0-based day-of-week.
	u32 yday; ///< 0-based day-of-year.
	char timezoneName[8]; ///< Timezone name string.
	u32 DST; ///< 0 = no DST, 1 = DST.
	s32 offset; ///< Seconds relative to UTC for this timezone.
} TimeCalendarAdditionalInfo;

typedef struct {
	char name[0x24];
} TimeLocationName;

typedef struct Thread {
	u32 handle;       ///< Thread handle.
	void* stack_mem;    ///< Pointer to stack memory.
	void* stack_mirror; ///< Pointer to stack memory mirror.
	size_t stack_sz;     ///< Stack size.
	void** tls_array;
	struct Thread* next;
	struct Thread** prev_next;
} Thread;

typedef struct {
	u64 uid[2];       ///< UserId. All-zero is invalid / Uid not set. See also \ref accountUidIsValid.
} AccountUid;

typedef enum
{
	SetLanguage_JA = 0,   ///< Japanese
	SetLanguage_ENUS = 1,   ///< US English ("AmericanEnglish")
	SetLanguage_FR = 2,   ///< French
	SetLanguage_DE = 3,   ///< German
	SetLanguage_IT = 4,   ///< Italian
	SetLanguage_ES = 5,   ///< Spanish
	SetLanguage_ZHCN = 6,   ///< Simplified Chinese ("Chinese")
	SetLanguage_KO = 7,   ///< Korean
	SetLanguage_NL = 8,   ///< Dutch
	SetLanguage_PT = 9,   ///< Portuguese
	SetLanguage_RU = 10,  ///< Russian
	SetLanguage_ZHTW = 11,  ///< Traditional Chinese ("Taiwanese")
	SetLanguage_ENGB = 12,  ///< GB English ("BritishEnglish")
	SetLanguage_FRCA = 13,  ///< CA French ("CanadianFrench")
	SetLanguage_ES419 = 14,  ///< "LatinAmericanSpanish"
	SetLanguage_ZHHANS = 15, ///< [4.0.0+] ChineseSimplified
	SetLanguage_ZHHANT = 16, ///< [4.0.0+] ChineseTraditional
	SetLanguage_Total,       ///< Total languages supported by this enum.
} SetLanguage;

#define accountUidIsValid(Uid) (Uid.uid[0] != 0 || Uid.uid[1] != 0)

#endif