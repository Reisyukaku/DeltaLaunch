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

#include "Time.hpp"

std::string Time::GetClock(ClockType type) {
    u64 time = 0;
	TimeCalendarTime calTime = {};
	TimeCalendarAdditionalInfo info = {};
#ifdef __SWITCH__
    timeGetCurrentTime(TimeType_Default, &time);
    timeToCalendarTimeWithMyRule(time, &calTime, &info);
#else
	SYSTEMTIME st;
	GetLocalTime(&st);
    calTime.hour = st.wHour;
    calTime.minute = st.wMinute;
#endif
    char buff[10];
    std::string ampm[] = {"AM", "PM"};
    snprintf(buff, sizeof(buff), "%02d:%02d %s", type == CLOCK_12HR ? calTime.hour % 12 : calTime.hour, calTime.minute, (type == CLOCK_12HR) ? ampm[calTime.hour > 12 ? 1 : 0].c_str() : "");
    return std::string(buff);
}

std::string Time::GetDate() {
    u64 time = 0;
	TimeCalendarTime calTime = {};
	TimeCalendarAdditionalInfo info = {};
#ifdef __SWITCH__
    timeGetCurrentTime(TimeType_Default, &time);
    timeToCalendarTimeWithMyRule(time, &calTime, &info);
#else
	SYSTEMTIME st;
	GetLocalTime(&st);
	calTime.day = st.wDay;
    calTime.month = st.wMonth;
    calTime.year = st.wYear;
#endif
    char buff[100];
    snprintf(buff, sizeof(buff), "%s %d, %d", Language::getInstance()->GetString((UiString)((u32)LANG_JAN + (calTime.month-1))).c_str(), calTime.day, calTime.year);
    return std::string(buff);
}

std::string Time::GetTimezoneName() {
	TimeLocationName tz = {};
    #ifdef __SWITCH__
    timeGetDeviceLocationName(&tz);
    #endif
    return std::string(tz.name);
}