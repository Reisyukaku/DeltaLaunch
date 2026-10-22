cmake_minimum_required(VERSION 3.16)
project(DeltaLaunch VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(APP_LIBS REQUIRED IMPORTED_TARGET freetype2 SDL2_mixer sdl2 libcurl)
add_library(delta_platform INTERFACE)

if(NINTENDO_SWITCH)
    # Switch.cmake supplies the architecture, libnx includes, specs and runtime.
    # Portlibs are static, so include the codecs/TLS libraries from pkg-config.
    target_include_directories(delta_platform INTERFACE ${APP_LIBS_INCLUDE_DIRS})
    target_compile_options(delta_platform INTERFACE ${APP_LIBS_CFLAGS_OTHER})
    target_link_directories(delta_platform INTERFACE ${APP_LIBS_STATIC_LIBRARY_DIRS})
    target_link_libraries(delta_platform INTERFACE glfw3 EGL glapi drm_nouveau ${APP_LIBS_STATIC_LIBRARIES})
    set(APP_TARGET deltalaunch)
else()
    pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
    find_package(Threads REQUIRED)
    target_compile_definitions(delta_platform INTERFACE DELTA_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(delta_platform INTERFACE PkgConfig::APP_LIBS PkgConfig::GLFW Threads::Threads ${CMAKE_DL_LIBS})
    set(APP_TARGET deltalaunch-linux)
endif()

file(GLOB APP_SOURCES CONFIGURE_DEPENDS source/*.cpp source/Core/*.cpp source/Services/*.cpp source/UI/*.cpp)
add_executable(${APP_TARGET} ${APP_SOURCES} source/Core/GL/glad.c)
target_link_libraries(${APP_TARGET} PRIVATE delta_platform)
target_compile_options(${APP_TARGET} PRIVATE -Wall -Wextra -Wno-unused-parameter)

if(NINTENDO_SWITCH)
    target_compile_options(${APP_TARGET} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
    target_compile_definitions(${APP_TARGET} PRIVATE TITLE="Delta Launch" $<$<CONFIG:Debug>:__DEBUG__>)
    target_link_options(${APP_TARGET} PRIVATE "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/deltalaunch.map")
    include(cmake/SwitchPackaging.cmake)
else()
    include(CTest)
    if(BUILD_TESTING)
        add_executable(libnx-linux-tests tests/libnx_linux.cpp source/Services/Hid.cpp)
        target_link_libraries(libnx-linux-tests PRIVATE delta_platform)
        add_test(NAME libnx-linux-contracts COMMAND libnx-linux-tests)
        find_program(XVFB_RUN xvfb-run)
        if(XVFB_RUN)
            add_test(NAME linux-render-smoke COMMAND ${XVFB_RUN} -a $<TARGET_FILE:deltalaunch-linux> --frames 5)
            set_tests_properties(linux-render-smoke PROPERTIES
                ENVIRONMENT "SDL_AUDIODRIVER=dummy;LIBGL_ALWAYS_SOFTWARE=1;DELTA_SD_ROOT=${CMAKE_CURRENT_BINARY_DIR}/smoke-sd"
                TIMEOUT 30)
        endif()
    endif()
endif()
