#pragma once
// A host test double for the libnx API used by DeltaLaunch, not a console emulator.
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <thread>
#include <vector>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;
using Result = u32;
#define BIT(n) (1U << (n))
#define R_SUCCEEDED(rc) ((rc) == 0)
#define R_FAILED(rc) ((rc) != 0)
inline constexpr Result NxStubUnsupported = 0xF001;
inline constexpr Result NxStubInvalidArgument = 0xF002;
inline constexpr u32 CUR_PROCESS_HANDLE = 0;

namespace nxstub {
inline const std::filesystem::path sourceRoot = DELTA_SOURCE_DIR;
inline std::filesystem::path sdRoot() {
    const char* root = std::getenv("DELTA_SD_ROOT");
    return root ? std::filesystem::path(root) : std::filesystem::current_path() / "SD";
}
inline std::vector<u8> readFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}
inline bool lockScreen = false;
inline std::array<char, 0x80> nickname = {'L','i','n','u','x'};
inline u64 buttons = 0; // Can be set by tests without opening a window.
inline bool windowInput = true;
}

enum HidNpadButton : u64 {
    HidNpadButton_A=BIT(0), HidNpadButton_B=BIT(1), HidNpadButton_X=BIT(2), HidNpadButton_Y=BIT(3),
    HidNpadButton_StickL=BIT(4), HidNpadButton_StickR=BIT(5), HidNpadButton_L=BIT(6), HidNpadButton_R=BIT(7),
    HidNpadButton_ZL=BIT(8), HidNpadButton_ZR=BIT(9), HidNpadButton_Plus=BIT(10), HidNpadButton_Minus=BIT(11),
    HidNpadButton_Left=BIT(12), HidNpadButton_Up=BIT(13), HidNpadButton_Right=BIT(14), HidNpadButton_Down=BIT(15),
    HidNpadButton_StickLLeft=BIT(16), HidNpadButton_StickLUp=BIT(17),
    HidNpadButton_StickLRight=BIT(18), HidNpadButton_StickLDown=BIT(19),
    HidNpadButton_StickRLeft=BIT(20), HidNpadButton_StickRUp=BIT(21),
    HidNpadButton_StickRRight=BIT(22), HidNpadButton_StickRDown=BIT(23),
    HidNpadButton_AnyLeft=HidNpadButton_Left|HidNpadButton_StickLLeft|HidNpadButton_StickRLeft,
    HidNpadButton_AnyUp=HidNpadButton_Up|HidNpadButton_StickLUp|HidNpadButton_StickRUp,
    HidNpadButton_AnyRight=HidNpadButton_Right|HidNpadButton_StickLRight|HidNpadButton_StickRRight,
    HidNpadButton_AnyDown=HidNpadButton_Down|HidNpadButton_StickLDown|HidNpadButton_StickRDown
};
inline constexpr u32 HidNpadStyleSet_NpadStandard = 0;
struct PadState { u64 held = 0, down = 0; };
struct HidTouchState { u32 finger_id=0, x=0, y=0, diameter_x=0, diameter_y=0, rotation_angle=0; };
struct HidTouchScreenState { u32 count=0; HidTouchState touches[16] = {}; };
namespace nxstub { inline HidTouchScreenState touchscreen = {}; }
inline void padConfigureInput(u32, u32) {}
inline void padInitializeDefault(PadState* pad) { *pad = {}; }
inline void padUpdate(PadState* pad) {
    u64 current = nxstub::buttons;
    if (nxstub::windowInput) {
        glfwPollEvents();
        if (auto* window = glfwGetCurrentContext()) {
            const std::pair<int,u64> keys[] = {
                {GLFW_KEY_ENTER,HidNpadButton_A}, {GLFW_KEY_ESCAPE,HidNpadButton_B},
                {GLFW_KEY_X,HidNpadButton_X}, {GLFW_KEY_Y,HidNpadButton_Y},
                {GLFW_KEY_LEFT,HidNpadButton_Left}, {GLFW_KEY_RIGHT,HidNpadButton_Right},
                {GLFW_KEY_UP,HidNpadButton_Up}, {GLFW_KEY_DOWN,HidNpadButton_Down},
                {GLFW_KEY_EQUAL,HidNpadButton_Plus}, {GLFW_KEY_MINUS,HidNpadButton_Minus},
                {GLFW_KEY_L,HidNpadButton_StickL}, {GLFW_KEY_Q,HidNpadButton_L}, {GLFW_KEY_E,HidNpadButton_R}
            };
            for (auto [key, button] : keys) if (glfwGetKey(window,key)==GLFW_PRESS) current |= button;
        }
    }
    pad->down = current & ~pad->held;
    pad->held = current;
}
inline u64 padGetButtonsDown(const PadState* pad) { return pad->down; }
inline void hidInitializeTouchScreen() {}
inline s32 hidGetTouchScreenStates(HidTouchScreenState* states, s32 count) {
    if (!states || count <= 0) return 0;
    *states = {};
    if (!nxstub::windowInput) { *states=nxstub::touchscreen; return 1; }
    if (auto* window = glfwGetCurrentContext()) {
        if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS) {
            double x,y; int width,height;
            glfwGetCursorPos(window,&x,&y); glfwGetWindowSize(window,&width,&height);
            if (width > 0 && height > 0) {
                states->count=1;
                states->touches[0].x=static_cast<u32>(std::clamp(x*1280/width,0.0,1279.0));
                states->touches[0].y=static_cast<u32>(std::clamp(y*720/height,0.0,719.0));
            }
        }
    }
    return 1;
}
inline u64 armGetSystemTick() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
inline u64 armTicksToNs(u64 ticks) { return ticks; }

enum SetLanguage { SetLanguage_JA, SetLanguage_ENUS, SetLanguage_FR, SetLanguage_DE, SetLanguage_IT,
    SetLanguage_ES, SetLanguage_ZHCN, SetLanguage_KO, SetLanguage_NL, SetLanguage_PT, SetLanguage_RU,
    SetLanguage_ZHTW, SetLanguage_ENGB, SetLanguage_FRCA, SetLanguage_ES419, SetLanguage_ZHHANS,
    SetLanguage_ZHHANT, SetLanguage_Total };
enum SetRegion { SetRegion_JPN, SetRegion_USA, SetRegion_EUR, SetRegion_AUS };
struct SetSysFirmwareVersion { char display_version[0x18]; };
struct SetSysSerialNumber { char number[0x18]; };
struct SetSysDeviceNickName { char nickname[0x80]; };
inline Result setInitialize() { return 0; }
inline void setExit() {}
inline Result setsysInitialize() { return 0; }
inline void setsysExit() {}
inline Result setGetSystemLanguage(u64* code) { *code=0x53552d6e65; return 0; } // en-US
inline Result setMakeLanguage(u64, SetLanguage* language) { *language=SetLanguage_ENUS; return 0; }
inline Result setGetRegionCode(SetRegion* region) { *region=SetRegion_USA; return 0; }
inline Result setsysGetFirmwareVersion(SetSysFirmwareVersion* out) { *out={}; std::strcpy(out->display_version,"Linux stub"); return 0; }
inline Result setsysGetSerialNumber(SetSysSerialNumber* out) { *out={}; std::strcpy(out->number,"LINUX000000000"); return 0; }
inline Result setsysGetDeviceNickname(SetSysDeviceNickName* out) { std::memcpy(out->nickname,nxstub::nickname.data(),sizeof(out->nickname)); return 0; }
inline Result setsysSetDeviceNickname(const SetSysDeviceNickName* in) {
    std::memcpy(nxstub::nickname.data(),in->nickname,nxstub::nickname.size()); nxstub::nickname.back()=0; return 0;
}
inline Result setsysGetLockScreenFlag(bool* out) { *out=nxstub::lockScreen; return 0; }
inline Result setsysSetLockScreenFlag(bool flag) { nxstub::lockScreen=flag; return 0; }

struct AccountUid { u64 uid[2]; };
struct AccountProfile { AccountUid uid; };
inline constexpr int AccountServiceType_System=0;
inline bool accountUidIsValid(const AccountUid* uid) { return uid && (uid->uid[0] || uid->uid[1]); }
inline Result accountInitialize(int) { return 0; }
inline void accountExit() {}
inline Result accountGetUserCount(s32* count) { *count=1; return 0; }
inline Result accountGetLastOpenedUser(AccountUid* uid) { *uid={{1,0}}; return 0; }
inline Result accountTrySelectUserWithoutInteraction(AccountUid* uid,bool) { return accountGetLastOpenedUser(uid); }
inline Result accountListAllUsers(AccountUid* users,s32 capacity,s32* count) {
    *count=capacity>0 ? 1:0; if (*count) users[0]={{1,0}}; return 0;
}
inline Result accountGetProfile(AccountProfile* profile,AccountUid uid) { profile->uid=uid; return 0; }
inline const std::vector<u8>& nxstubIcon() {
    static const auto image=nxstub::readFile(nxstub::sourceRoot/"romfs/Graphics/DummyGame.png"); return image;
}
inline Result accountProfileGetImageSize(AccountProfile*,u32* size) { *size=nxstubIcon().size(); return 0; }
inline Result accountProfileLoadImage(AccountProfile*,void* buffer,size_t capacity,u32* size) {
    *size=std::min(capacity,nxstubIcon().size());
    if (*size) std::memcpy(buffer,nxstubIcon().data(),*size);
    return 0;
}
inline void accountProfileClose(AccountProfile*) {}
struct NacpLanguageEntry { char name[0x200], author[0x100]; };
struct NacpStruct { NacpLanguageEntry lang[16]; char display_version[0x10]; };
struct NsApplicationControlData { NacpStruct nacp; u8 icon[0x20000]; };
struct NsApplicationRecord { u64 application_id; u8 last_event; };
enum NsApplicationControlSource { NsApplicationControlSource_Storage };
inline Result nsInitialize() { return 0; }
inline void nsExit() {}
inline Result nsListApplicationRecord(NsApplicationRecord* records,s32 capacity,s32 offset,s32* count) {
    *count=0;
    if (capacity<0 || offset<0) return NxStubInvalidArgument;
    *count=std::max(0,std::min(capacity,3-offset));
    for (s32 i=0;i<*count;i++) records[i]={0x0100000000000000ULL+static_cast<u64>(offset+i),3};
    return 0;
}
inline Result nsGetApplicationControlData(NsApplicationControlSource,u64 id,void* buffer,size_t capacity,size_t* actual) {
    *actual=sizeof(NsApplicationControlData);
    if (capacity < *actual) return NxStubInvalidArgument;
    auto* out=static_cast<NsApplicationControlData*>(buffer); *out={};
    std::snprintf(out->nacp.lang[0].name,sizeof(out->nacp.lang[0].name),"Linux demo %u",unsigned(id&0xffff)+1);
    std::strcpy(out->nacp.lang[0].author,"DeltaLaunch host stubs");
    std::strcpy(out->nacp.display_version,"0.0.0");
    std::memcpy(out->icon,nxstubIcon().data(),std::min(sizeof(out->icon),nxstubIcon().size()));
    return 0;
}
inline Result nacpGetLanguageEntry(NacpStruct* nacp,NacpLanguageEntry** entry) { *entry=&nacp->lang[0]; return 0; }

inline constexpr int PlSharedFontType_Total=6, PlServiceType_System=0;
struct PlFontData { u32 type,offset,size; void* address; };
inline Result plInitialize(int) { return 0; }
inline void plExit() {}
inline Result plGetSharedFont(u64,PlFontData* fonts,s32 capacity,s32* count) {
    static const auto data=[] {
        std::array<std::vector<u8>,6> buffers;
        const char* names[]={"FontStandard.ttf","FontChineseSimplified.ttf","FontExtendedChineseSimplified.ttf",
            "FontChineseTraditional.ttf","FontKorean.ttf","FontNintendoExtended.ttf"};
        for (size_t i=0;i<buffers.size();i++) buffers[i]=nxstub::readFile(nxstub::sourceRoot/"project/Fonts"/names[i]);
        return buffers;
    }();
    *count=std::min(capacity,PlSharedFontType_Total);
    for (s32 i=0;i<*count;i++) {
        fonts[i]={static_cast<u32>(i),0,static_cast<u32>(data[i].size()),const_cast<u8*>(data[i].data())};
        if (data[i].empty()) { *count=0; return NxStubUnsupported; }
    }
    return 0;
}

enum TimeType { TimeType_Default, TimeType_UserSystemClock };
struct TimeCalendarTime { s16 year; s8 month,day,hour,minute,second; };
struct TimeCalendarAdditionalInfo { u32 wday,yday; char timezoneName[8]; u32 DST; s32 offset; };
struct TimeLocationName { char name[0x24]; };
inline Result timeGetCurrentTime(TimeType,u64* timestamp) { *timestamp=std::time(nullptr); return 0; }
inline Result timeToCalendarTimeWithMyRule(u64 timestamp,TimeCalendarTime* calendar,TimeCalendarAdditionalInfo* info) {
    std::time_t seconds=timestamp; std::tm local={};
    if (!localtime_r(&seconds,&local)) return NxStubInvalidArgument;
    *calendar={static_cast<s16>(local.tm_year+1900),static_cast<s8>(local.tm_mon+1),static_cast<s8>(local.tm_mday),
        static_cast<s8>(local.tm_hour),static_cast<s8>(local.tm_min),static_cast<s8>(local.tm_sec)};
    *info={}; info->wday=local.tm_wday; info->yday=local.tm_yday; info->DST=local.tm_isdst>0;
    std::strftime(info->timezoneName,sizeof(info->timezoneName),"%Z",&local); return 0;
}
inline Result timeGetDeviceLocationName(TimeLocationName* out) {
    *out={}; std::time_t now=std::time(nullptr); std::tm local={}; localtime_r(&now,&local);
    std::strftime(out->name,sizeof(out->name),"%Z",&local); return 0;
}

struct Service { bool active; };
inline bool serviceIsActive(const Service* service) { return service->active; }
inline Result smGetService(Service* service,const char*) { *service={}; return NxStubUnsupported; }
inline void serviceClose(Service* service) { *service={}; }
inline Result serviceDispatchIn(Service*,u32,u64) { return NxStubUnsupported; }
struct FsStorage {};
struct FsFileSystem {};
struct FsDeviceOperator {};
inline constexpr int FsBisPartitionId_BootPartition1Root=0;
inline Result fsOpenBisStorage(FsStorage*,int) { return NxStubUnsupported; }
inline Result fsStorageGetSize(FsStorage*,s64* size) { *size=0; return NxStubUnsupported; }
inline Result fsStorageRead(FsStorage*,s64,void*,u64) { return NxStubUnsupported; }
inline Result fsStorageWrite(FsStorage*,s64,const void*,u64) { return NxStubUnsupported; }
inline void fsStorageClose(FsStorage*) {}
inline Result fsOpen_SaveData(FsFileSystem*,u64,AccountUid) { return NxStubUnsupported; }
inline int fsdevMountDevice(const char*,FsFileSystem) { return -1; }
inline int fsdevUnmountDevice(const char*) { return -1; }
inline Result fsOpenDeviceOperator(FsDeviceOperator*) { return 0; }
inline Result fsDeviceOperatorIsGameCardInserted(FsDeviceOperator*,bool* inserted) { *inserted=false; return 0; }
inline Result romfsMountFromFsdev(const char*,u64,const char*) { return 0; } // Assets resolved by Platform::Resources.
inline void romfsExit() {}
inline constexpr int NifmServiceType_System=0;
enum NifmInternetConnectionType { NifmInternetConnectionType_WiFi };
enum NifmInternetConnectionStatus { NifmInternetConnectionStatus_Disconnected };
inline Result nifmInitialize(int) { return 0; }
inline void nifmExit() {}
inline Result nifmGetInternetConnectionStatus(NifmInternetConnectionType* type,u32* strength,NifmInternetConnectionStatus* status) {
    *type=NifmInternetConnectionType_WiFi; *strength=0; *status=NifmInternetConnectionStatus_Disconnected; return 0;
}
inline Result psmInitialize() { return 0; }
inline void psmExit() {}
inline Result psmGetBatteryChargePercentage(u32* percent) { *percent=100; return 0; }
struct GpioPadSession {};
using GpioPadName = u32;
enum GpioValue { GpioValue_Low, GpioValue_High };
inline Result gpioOpenSession(GpioPadSession*,GpioPadName) { return NxStubUnsupported; }
inline Result gpioPadGetValue(GpioPadSession*,GpioValue* value) { *value=GpioValue_High; return NxStubUnsupported; }
inline void gpioPadClose(GpioPadSession*) {}

// Thread is deliberately POD: libnx callers zero-initialize it with memset.
struct Thread { void (*entry)(void*); void* argument; std::thread* worker; };
inline Result threadCreate(Thread* thread,void (*entry)(void*),void* argument,void*,size_t,int,int) {
    *thread={entry,argument,nullptr}; return 0;
}
inline Result threadStart(Thread* thread) {
    if (!thread->entry || thread->worker) return NxStubInvalidArgument;
    thread->worker=new std::thread(thread->entry,thread->argument); return 0;
}
inline Result threadWaitForExit(Thread* thread) {
    if (thread->worker && thread->worker->joinable()) thread->worker->join();
    return 0;
}
inline Result threadClose(Thread* thread) { threadWaitForExit(thread); delete thread->worker; *thread={}; return 0; }
inline s32 svcGetCurrentProcessorNumber() { return 0; }
inline void svcSleepThread(s64 ns) { std::this_thread::sleep_for(std::chrono::nanoseconds(ns)); }
inline Result svcGetInfo(u64* value,u32,u32,u64) { *value=0; return NxStubUnsupported; }
struct Event {};
inline Result eventWait(Event*,u64) { return NxStubUnsupported; }
inline void eventClose(Event*) {}

inline constexpr u32 AppletType_SystemApplet=3;
enum AppletId { AppletId_LibraryAppletPhotoViewer, AppletId_LibraryAppletPlayerSelect,
    AppletId_LibraryAppletShop, AppletId_LibraryAppletNetConnect, AppletId_LibraryAppletController,
    AppletId_LibraryAppletError, AppletId_LibraryAppletOfflineWeb };
enum LibAppletMode { LibAppletMode_AllForeground };
enum AppletLaunchParameterKind { AppletLaunchParameterKind_PreselectedUser };
struct AppletApplication { u64 id; };
struct AppletHolder { AppletId id; };
struct AppletStorage { std::vector<u8> data; };
struct LibAppletArgs {};
inline Result appletCreateStorage(AppletStorage* storage,s64 size) {
    storage->data.clear(); if (size<0) return NxStubInvalidArgument;
    storage->data.resize(size); return 0;
}
inline Result appletStorageWrite(AppletStorage* storage,s64 offset,const void* data,size_t size) {
    if (offset<0 || static_cast<u64>(offset)>storage->data.size() || size>storage->data.size()-offset) return NxStubInvalidArgument;
    if (size) std::memcpy(storage->data.data()+offset,data,size);
    return 0;
}
inline Result appletStorageRead(AppletStorage* storage,s64 offset,void* data,size_t size) {
    if (offset<0 || static_cast<u64>(offset)>storage->data.size() || size>storage->data.size()-offset) {
        if (size) std::memset(data,0,size);
        return NxStubInvalidArgument;
    }
    if (size) std::memcpy(data,storage->data.data()+offset,size);
    return 0;
}
inline Result appletStorageGetSize(AppletStorage* storage,s64* size) { *size=storage->data.size(); return 0; }
inline void appletStorageClose(AppletStorage* storage) { storage->data.clear(); }
inline Result appletCreateApplication(AppletApplication* app,u64 id) { *app={id}; return NxStubUnsupported; }
inline Result appletApplicationPushLaunchParameter(AppletApplication*,AppletLaunchParameterKind,AppletStorage*) { return NxStubUnsupported; }
inline Result appletApplicationStart(AppletApplication*) { return NxStubUnsupported; }
inline Result appletApplicationRequestForApplicationToGetForeground(AppletApplication*) { return NxStubUnsupported; }
inline void appletApplicationClose(AppletApplication*) {}
inline Result appletCreateLibraryApplet(AppletHolder* holder,AppletId id,LibAppletMode) { *holder={id}; return 0; }
inline Result appletHolderPushInData(AppletHolder*,AppletStorage*) { return 0; }
inline Result appletHolderStart(AppletHolder* holder) {
    if (holder->id==AppletId_LibraryAppletPlayerSelect) return 0;
    if (holder->id==AppletId_LibraryAppletError) std::fputs("[libnx stub] Console error applet requested\n",stderr);
    return NxStubUnsupported;
}
inline void appletHolderJoin(AppletHolder*) {}
inline void appletHolderClose(AppletHolder*) {}
inline Result appletHolderRequestExit(AppletHolder*) { return 0; }
inline Result appletHolderPopOutData(AppletHolder* holder,AppletStorage* storage) {
    storage->data.clear();
    if (holder->id!=AppletId_LibraryAppletPlayerSelect) return NxStubUnsupported;
    appletCreateStorage(storage,0x18); AccountUid uid={{1,0}};
    return appletStorageWrite(storage,8,&uid,sizeof(uid));
}
inline void libappletArgsCreate(LibAppletArgs*,u32) {}
inline void libappletArgsSetPlayStartupSound(LibAppletArgs*,bool) {}
inline Result libappletArgsPush(LibAppletArgs*,AppletHolder*) { return 0; }
inline Result appletUnlockForeground() { return 0; }
inline Result appletRequestToGetForeground() { return 0; }
inline Result appletSetHandlesRequestToDisplay(bool) { return 0; }
inline Result appletLoadAndApplyIdlePolicySettings() { return 0; }
inline Result appletStartShutdownSequence() { return NxStubUnsupported; }
inline Result appletStartRebootSequence() { return NxStubUnsupported; }
inline Result appletStartSleepSequence(bool) { return NxStubUnsupported; }
inline Result appletGetPopFromGeneralChannelEvent(Event*) { return NxStubUnsupported; }
inline Result appletPopFromGeneralChannel(AppletStorage* storage) { storage->data.clear(); return NxStubUnsupported; }
inline Result appletGetMessage(u32* message) { *message=0; return NxStubUnsupported; }
inline u8 appletGetOperationMode() { return 0; }
struct WebCommonConfig {};
struct WebCommonReply {};
enum WebExitReason { WebExitReason_ExitButton };
inline Result webPageCreate(WebCommonConfig*,const char*) { return NxStubUnsupported; }
inline Result webConfigShow(WebCommonConfig*,WebCommonReply*) { return NxStubUnsupported; }
inline Result webReplyGetExitReason(WebCommonReply*,WebExitReason* reason) { *reason=WebExitReason_ExitButton; return 0; }
struct SwkbdConfig { std::string text; };
inline Result swkbdCreate(SwkbdConfig* config,s32) { config->text.clear(); return 0; }
inline void swkbdConfigMakePresetDefault(SwkbdConfig*) {}
inline void swkbdConfigSetHeaderText(SwkbdConfig*,const char*) {}
inline void swkbdConfigSetOkButtonText(SwkbdConfig*,const char*) {}
inline void swkbdConfigSetGuideText(SwkbdConfig*,const char*) {}
inline void swkbdConfigSetInitialText(SwkbdConfig* config,const char* text) { config->text=text; }
inline Result swkbdShow(SwkbdConfig* config,char* text,size_t capacity) {
    // Tests may supply text; interactive native keyboard editing is not emulated.
    const char* value=std::getenv("DELTA_KEYBOARD_TEXT");
    if (!capacity) return NxStubInvalidArgument;
    std::snprintf(text,capacity,"%s",value ? value : config->text.c_str()); return 0;
}
inline void swkbdClose(SwkbdConfig*) {}
inline void consoleUpdate(void*) {}
