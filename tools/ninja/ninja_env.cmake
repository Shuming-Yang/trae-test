# ninja_env.cmake - Toolchain File for Ninja
# ====================== toolchain =====================
# 設定目標作業系統和架構
# SET(CMAKE_GENERATOR "Ninja")

IF(UNIX AND NOT APPLE)
    # MESSAGE(STATUS "LINUX")
    SET(NINJAEXT                   "")
    SET(CMAKE_MAKE_PROGRAM_PATH    "${CMAKE_SOURCE_DIR}\\tools\\ninja\\linux")
ELSEIF(UNIX AND APPLE)
    # MESSAGE(STATUS "MACOS")
    SET(NINJAEXT                   "")
    SET(CMAKE_MAKE_PROGRAM_PATH    "${CMAKE_SOURCE_DIR}\\tools\\ninja\\mac")
ELSEIF(WIN32)
    # MESSAGE(STATUS "WINDOWS")
    SET(NINJAEXT                   ".exe")
    SET(CMAKE_MAKE_PROGRAM_PATH    "${CMAKE_SOURCE_DIR}\\tools\\ninja\\win")
ELSE()
    MESSAGE(FATAL_ERROR "Not Support in Unknow OS")
ENDIF()

SET(CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM_PATH}/ninja${NINJAEXT}" CACHE FILEPATH "Path to ninja executable")
SET(ENV(PATH) "${CMAKE_MAKE_PROGRAM_PATH};$ENV{PATH}")
# 設定 Ninja 輸出格式：[已耗時 | 百分比 | 已完成/總數]
SET(ENV{NINJA_STATUS} "[%r | %p | %f/%t] ")
# ====================== toolchain =====================