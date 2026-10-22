#pragma once
#include <string>

// The only application-level platform selection lives here.
#ifdef __SWITCH__
#include <switch.h>
namespace Platform {
inline std::string Resources(const std::string& path) { return "romfs:" + path; }
inline std::string SdPath(const std::string& path) { return path; }
// libnx invokes these hooks before/after main itself.
class Runtime {
public:
    Runtime(void (*)(), void (*)()) {}
};
}
#else
#include "LibnxLinux.hpp"
namespace Platform {
inline std::string Resources(const std::string& path) {
    return (nxstub::sourceRoot / "romfs" / std::filesystem::path(path).relative_path()).string();
}
inline std::string SdPath(const std::string& path) {
    return (nxstub::sdRoot() / std::filesystem::path(path).relative_path()).string();
}
class Runtime {
    void (*shutdown)();
public:
    Runtime(void (*initialize)(), void (*finalize)()) : shutdown(finalize) {
        std::filesystem::create_directories(nxstub::sdRoot() / "ReiNX");
        std::filesystem::create_directories(nxstub::sdRoot() / "Themes");
        initialize();
    }
    ~Runtime() { shutdown(); }
    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;
};
}
#endif
