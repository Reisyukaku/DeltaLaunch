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
#include <stdlib.h>
#include "Power.hpp"
#include "../Core/Graphics.hpp"
#include "../Core/GL/GL.hpp"
#include "../Core/types.h"

#define TAP_MOVEMENT_GAP 20
#define LONG_TAP_PERIOD 1.5

#ifdef __SWITCH__
#define tapped_inside(touchInfo, x1, y1, x2, y2) (touchInfo.firstTouch.px >= x1 && touchInfo.firstTouch.px <= x2 && touchInfo.firstTouch.py >= y1 && touchInfo.firstTouch.py <= y2)
#define tapped_outside(touchInfo, x1, y1, x2, y2) (touchInfo.firstTouch.px < x1 || touchInfo.firstTouch.px > x2 || touchInfo.firstTouch.py < y1 || touchInfo.firstTouch.py > y2)
#else
#define tapped_inside(touchInfo, x1, y1, x2, y2) (false)
#define tapped_outside(touchInfo, x1, y1, x2, y2) (false)
#endif

class TouchInfo {
    public:
        typedef enum TouchState 
        {
            TouchNone,
            TouchStart,
            TouchMoving,
            TouchEnded
        } TouchState;

        typedef enum TapType 
        {
            TapNone,
            TapShort,
            TapLong
        } TapType;
    
        TouchState state;
        TapType tapType;
        u64 touchStart;
		touchPosition firstTouch;
		touchPosition prevTouch;
};

class Hid {
    public:
        static u64 Input;
        static void KeyProcess();
        static void TouchInit();
        static void TouchProcess();
        static bool IsTouched(Rect pos) { return (tapped_inside(touchInfo, (unsigned)pos.x, (unsigned)pos.y, (unsigned)(pos.x+pos.w), (unsigned)(pos.y+pos.h)) && touchInfo.state == TouchInfo::TouchStart); };
        static bool IsTouched() { return (touchInfo.tapType == TouchInfo::TapShort && touchInfo.state == TouchInfo::TouchStart);  };
		static bool IsLongPress() { return (touchInfo.tapType == TouchInfo::TapLong && touchInfo.state == TouchInfo::TouchStart); };
        static bool IsMoving() { return touchInfo.state == TouchInfo::TouchMoving; }
        static bool IsMoving(Rect pos) { return (tapped_inside(touchInfo, (unsigned)pos.x, (unsigned)pos.y, (unsigned)(pos.x+pos.w), (unsigned)(pos.y+pos.h)) && touchInfo.state == TouchInfo::TouchMoving); }
		static touchPosition GetTouchPos() { return touchInfo.prevTouch; };
#ifdef __SWITCH__
		static bool IsGpioPressed(GpioPadSession *but, GpioPadName name);
		static void InitGpioButton(GpioPadSession *but, GpioPadName name);
		static void CloseGpioButton(GpioPadSession *but);
#endif
    private:
        static TouchInfo touchInfo;
};