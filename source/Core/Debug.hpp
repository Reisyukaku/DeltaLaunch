#pragma once

#include <vector>
#include <string>
#include "types.h"
#include <cstdarg>

class Debug {
public:
	enum class Type {
		NONE,
		DASHBOARD,
		AUDIO,
		ENGINE,
		FONT,
		APPLET,
		GFX,
		GL,
		HID,
		NETWORK,
		SETTINGS,
		THREAD,
		GAME,
		SHADER
	};

	static void Write(unsigned col, Type type, std::string fmt, ...) {

		va_list args;
		char string[0x100];
		va_start(args, fmt);
		vsnprintf(string, 0x100, fmt.c_str(), args);
		va_end(args);

		std::vector<std::string> MsgHdr{ "", "Dashboard", "Audio" , "Engine", "Font", "Applet" ,"Gfx" ,"GLFW" ,"Hid" ,"Network" ,"Settings", "ThreadManager", "Game", "Shader" };
		printf("[%s]: %s", MsgHdr[(int)type].c_str(), string);

	}
};