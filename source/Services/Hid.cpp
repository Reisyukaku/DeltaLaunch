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

#include "Hid.hpp"

u64 Hid::Input;
TouchInfo Hid::touchInfo;

void Hid::KeyProcess() {
#ifdef __SWITCH__
    hidScanInput();
    Input = hidKeysDown(CONTROLLER_P1_AUTO);
#else
	Input = 0;
	while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) Input |= KEY_B;
	while (GetAsyncKeyState(VK_RETURN) & 0x8000) Input |= KEY_A;
	while (GetAsyncKeyState('y') & 0x8000) Input |= KEY_Y;
	while (GetAsyncKeyState('x') & 0x8000) Input |= KEY_X;
	while (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) Input |= KEY_PLUS;
	while (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) Input |= KEY_MINUS;
	while (GetAsyncKeyState(VK_LEFT) & 0x8000) Input |= KEY_LEFT;
	while (GetAsyncKeyState(VK_UP) & 0x8000) Input |= KEY_UP;
	while (GetAsyncKeyState(VK_RIGHT) & 0x8000) Input |= KEY_RIGHT;
	while (GetAsyncKeyState(VK_DOWN) & 0x8000) Input |= KEY_DOWN;
	if (Input != 0) {
		switch (Input) {
		case KEY_B: DEBUG(Debug::Type::HID, "Button press: B\n"); break;
		case KEY_A: DEBUG(Debug::Type::HID, "Button press: A\n"); break;
		case KEY_Y: DEBUG(Debug::Type::HID, "Button press: Y\n"); break;
		case KEY_X: DEBUG(Debug::Type::HID, "Button press: X\n"); break;
		case KEY_PLUS: DEBUG(Debug::Type::HID, "Button press: +\n"); break;
		case KEY_MINUS: DEBUG(Debug::Type::HID, "Button press: -\n"); break;
		case KEY_LEFT: DEBUG(Debug::Type::HID, "Button press: Left\n"); break;
		case KEY_UP: DEBUG(Debug::Type::HID, "Button press: Up\n"); break;
		case KEY_RIGHT: DEBUG(Debug::Type::HID, "Button press: Right\n"); break;
		case KEY_DOWN: DEBUG(Debug::Type::HID, "Button press: Down\n"); break;
		}
	}
	
#endif
}

void Hid::TouchInit() {
    touchInfo.state = TouchInfo::TouchNone;
    touchInfo.tapType = TouchInfo::TapNone;
}

void Hid::TouchProcess() {
	touchPosition currentTouch = {};
	u64 current_time = 0;
#ifdef __SWITCH__
    u32 touches = hidTouchCount();
	if (touches >= 1)
		hidTouchRead(&currentTouch, 0);

	timeGetCurrentTime(TimeType_UserSystemClock, &current_time);
#else
	u32 touches = 0;
	//TODO
#endif
    
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

#ifdef __SWITCH__
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
#endif