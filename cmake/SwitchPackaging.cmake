# Use devkitPro's installed helpers for NACP, NRO, and the system applet ExeFS.
include(dkp-asset-folder)
file(GLOB_RECURSE ROMFS_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/romfs/*")
file(GLOB_RECURSE ROMFS_RELATIVE CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/romfs" "${CMAKE_CURRENT_SOURCE_DIR}/romfs/*")
dkp_add_asset_target(delta_romfs "${CMAKE_CURRENT_SOURCE_DIR}/romfs")
dkp_track_assets(delta_romfs FILES ${ROMFS_RELATIVE})
nx_generate_nacp(OUTPUT deltalaunch.nacp NAME "Delta Launch" AUTHOR "Reisyukaku" VERSION "${PROJECT_VERSION}")
nx_create_nro(deltalaunch NACP deltalaunch.nacp ROMFS delta_romfs)
nx_create_exefs(deltalaunch CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/deltalaunch.json")

find_program(BUILD_ROMFS_EXECUTABLE build_romfs HINTS "${DEVKITPRO}/tools/bin" REQUIRED)
set(DELTA_CONTENTS "${CMAKE_CURRENT_BINARY_DIR}/dist/0100000000001000")
add_custom_command(
    OUTPUT "${DELTA_CONTENTS}/exefs.nsp" "${DELTA_CONTENTS}/romfs.bin"
        "${CMAKE_CURRENT_BINARY_DIR}/deltalaunch.nso" "${CMAKE_CURRENT_BINARY_DIR}/deltalaunch.npdm"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DELTA_CONTENTS}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/deltalaunch.nsp" "${DELTA_CONTENTS}/exefs.nsp"
    COMMAND "${BUILD_ROMFS_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/romfs" "${DELTA_CONTENTS}/romfs.bin"
    COMMAND "${NX_ELF2NSO_EXE}" "$<TARGET_FILE:deltalaunch>" "${CMAKE_CURRENT_BINARY_DIR}/deltalaunch.nso"
    COMMAND "${NX_NPDMTOOL_EXE}" "${CMAKE_CURRENT_SOURCE_DIR}/deltalaunch.json" "${CMAKE_CURRENT_BINARY_DIR}/deltalaunch.npdm"
    DEPENDS deltalaunch_nsp "${CMAKE_CURRENT_BINARY_DIR}/deltalaunch.nsp" ${ROMFS_FILES}
    COMMENT "Packaging DeltaLaunch system applet"
    VERBATIM)
add_custom_target(deltalaunch_package ALL DEPENDS
    "${DELTA_CONTENTS}/exefs.nsp" "${DELTA_CONTENTS}/romfs.bin"
    "${CMAKE_CURRENT_BINARY_DIR}/deltalaunch.nso" "${CMAKE_CURRENT_BINARY_DIR}/deltalaunch.npdm")
