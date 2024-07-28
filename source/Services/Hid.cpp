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

#include <Services/Hid.hpp>

u64 Hid::Input;
TouchInfo Hid::touchInfo;

void Hid::KeyProcess() {
    hidScanInput();
    Input = hidKeysDown(CONTROLLER_P1_AUTO);
}

void Hid::TouchInit() {
    touchInfo.state = TouchInfo::TouchNone;
    touchInfo.tapType = TouchInfo::TapNone;
}

void Hid::TouchProcess() {
	touchPosition currentTouch = {};
	u64 current_time = 0;

    u32 touches = hidTouchCount();
	if (touches >= 1)
		hidTouchRead(&currentTouch, 0);

	timeGetCurrentTime(TimeType_UserSystemClock, &current_time);

    
    // On touch start.
    if (touches == 1 && (touchInfo.state == TouchInfo::TouchNone || touchInfo.state == TouchInfo::TouchEnded)) {
        touchInfo.state = TouchInfo::TouchStart;
        touchInfo.firstTouch = currentTouch;
        touchInfo.prevTouch = currentTouch;
        touchInfo.tapType = TouchInfo::TapShort;
        touchInfo.touchStart = current_time;
    }
    // On touch moving.
    else if (touches >= 1 && touchInfo.state != TouchInfo::TouchNone) {
        touchInfo.state = TouchInfo::TouchMoving;
        touchInfo.prevTouch = currentTouch;

        if (touchInfo.tapType != TouchInfo::TapNone && (abs((double)(touchInfo.firstTouch.px - currentTouch.px)) > TAP_MOVEMENT_GAP || abs((double)(touchInfo.firstTouch.py - currentTouch.py)) > TAP_MOVEMENT_GAP))
            touchInfo.tapType = TouchInfo::TapNone;
        else if (touchInfo.tapType == TouchInfo::TapShort && current_time - touchInfo.touchStart >= LONG_TAP_PERIOD)
            touchInfo.tapType = TouchInfo::TapLong;
    }
    // On touch end.
    else
        touchInfo.state = (touchInfo.state == TouchInfo::TouchMoving) ? TouchInfo::TouchEnded : TouchInfo::TouchNone;
}

bool Hid::IsGpioPressed(GpioPadSession *but, GpioPadName name) {
    GpioValue gval = GpioValue_High;
    gpioPadGetValue(but, &gval);
    return (gval == GpioValue_Low);
}

void Hid::InitGpioButton(GpioPadSession *but, GpioPadName name) {
	gpioOpenSession(but, name);
}

void Hid::CloseGpioButton(GpioPadSession *but) {
	gpioPadClose(but);
}