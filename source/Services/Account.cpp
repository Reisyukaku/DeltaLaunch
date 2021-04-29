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

#include "Account.hpp"

s32 Account::GetUserCount() {
    s32 cnt = 0;
    #ifdef __SWITCH__
    accountGetUserCount(&cnt);
    #endif
    return cnt;
}

AccountUid Account::GetActiveAccount() {
	AccountUid userID;
    #ifdef __SWITCH__
	Result rc = accountGetLastOpenedUser(&userID);
    if(R_FAILED(rc) || !accountUidIsValid(&userID))
        userID = App::LaunchPSelect();
    #endif
	return userID;
}

#define MAX_USERS 8
AccountUid Account::GetFirstAccount() {
    AccountUid userIDs[MAX_USERS];
    s32 total = 0;
    #ifdef __SWITCH__
    accountInitialize(AccountServiceType_System);
    accountListAllUsers(userIDs, MAX_USERS, &total);
    #endif
    return userIDs[0];
}

Result Account::SetCustomProfileImage(std::string filename) {
	Result rc = -1;
	std::ifstream pfp(filename.c_str());
	if (pfp.good()) {
/*#ifdef __SWITCH__
		accountInitialize(AccountServiceType_System);
		AccountUid uid = Account::GetFirstAccount();
		size_t fsize = 0;
		pfp.seekg(0, pfp.end);
		fsize = pfp.tellg();
		pfp.seekg(0, pfp.beg);
		char *buf = new char[fsize]{0};
		pfp.read(buf, fsize);
		pfp.close();
		AccountProfile acc;
		AccountProfileBase pb;
		AccountUserData ud;
		rc = accountGetProfile(&acc, uid);
		if (R_FAILED(rc)) return rc;
		rc = accountProfileGet(&acc, &ud, &pb);
		if (R_FAILED(rc)) return rc;
		ud.iconID = 0;

		rc = accountGetProfileEditor(&acc, uid);
		if (R_FAILED(rc)) return rc;
		rc = accStoreWithImage(&acc, &pb, &ud, (u8*)buf, fsize);
		accountProfileClose(&acc);
		delete buf;
#else*/
		rc = 0;
//#endif
	}
    return rc;
}

AccountUid Account::TryGetUser() {
    AccountUid userID;
    #ifdef __SWITCH__
    Result rc = accountTrySelectUserWithoutInteraction(&userID, false);
    if(R_FAILED(rc) || !accountUidIsValid(&userID))
        userID = App::LaunchPSelect();
    #endif
    return userID;
}

Texture *Account::GetProfileImage(AccountUid userID) {
	Texture *tex = nullptr;
#ifdef __SWITCH__
    if(accountUidIsValid(&userID)) {
        AccountProfile acc;
        u32 imgSize = 0;
        accountGetProfile(&acc, userID);
        accountProfileGetImageSize(&acc, &imgSize);
        void *buf = malloc(imgSize);
        accountProfileLoadImage(&acc, buf, imgSize, &imgSize);
        tex = Graphics::CreateTexFromBuffer(buf, imgSize);
        accountProfileClose(&acc);
    }
#else
	tex = new Texture;
#endif
    return tex;
}