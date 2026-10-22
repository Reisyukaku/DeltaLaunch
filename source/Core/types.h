#pragma once
#include <string>
#include "../Platform/Platform.hpp"
#include "Debug.hpp"
#define RESOURCES(x) Platform::Resources(x)
#define SD(x) Platform::SdPath(x)
#ifdef DELTA_TRACE
#define DEBUG(...) Debug::Write(0, __VA_ARGS__)
#else
#define DEBUG(...) ((void)0)
#endif
#define ERROR(...) Debug::Write(0, __VA_ARGS__)
