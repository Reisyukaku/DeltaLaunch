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

#include "Rnx.hpp"

#ifdef __SWITCH__
static Service g_rnxSrv;
#endif

bool Rnx::IsUsingReiNX() {
    #ifdef __SWITCH__
    if (serviceIsActive(&g_rnxSrv)) return true;
    #endif
    return false;
}

Result Rnx::Initialize() {
    Result rc = 0;
    #ifdef __SWITCH__
    if (serviceIsActive(&g_rnxSrv)) return 0;
    rc = smGetService(&g_rnxSrv, "rnx");
    #endif
    return rc;
}

void Rnx::Exit() {
    #ifdef __SWITCH__
    serviceClose(&g_rnxSrv);
    #endif
}

Result Rnx::SetHbTidForDelta(u64 tid) {
    Result rc = 0;
    #ifdef __SWITCH__
    rc = serviceDispatchIn(&g_rnxSrv, 0, tid);
    #endif
    return rc;
}