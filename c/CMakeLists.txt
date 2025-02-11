# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

cmake_minimum_required(VERSION 3.20)

project(evidense-commandline LANGUAGES C)

set(TOP ${PROJECT_SOURCE_DIR})

if(EXISTS "${PROJECT_SOURCE_DIR}/.isrepo.txt")
  set(COMMOM        ${TOP}/../../common/module/commandline)
  set(COMMOM_LIB    ${COMMOM}/lib)
  set(COMMOM_CMD    ${COMMOM}/cmd)
  set(COMMOM_3PARTY ${COMMOM}/3party)
  set(FW_COMMON     ${TOP}/../../common/module/firmware/application)
  set(FW            ${TOP}/../firmware/application/src)
else()
  set(COMMOM        ${TOP}/src)
  set(COMMOM_LIB    ${TOP}/src)
  set(COMMOM_CMD    ${TOP}/src)
  set(COMMOM_3PARTY ${TOP}/3party)
  set(FW_COMMON     ${TOP}/src)
  set(FW            ${TOP}/src)
endif()

message("TOP DIR    : ${TOP}")
message("COMMOM DIR : ${COMMOM}")

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

add_library(libevidense SHARED)
target_include_directories(libevidense PRIVATE "${COMMOM_LIB}" "${PROJECT_SOURCE_DIR}/src" "${FW_COMMON}" "${FW}")

target_sources(libevidense PRIVATE
  ${COMMOM_LIB}/evibase.h
  ${COMMOM_LIB}/evibase.c
  ${COMMOM_LIB}/crc-16-ccitt.c
  src/quadruple.c
  src/quadruple.h
  src/channel.c
  src/channel.h
  src/singlemeasurement.c
  src/singlemeasurement.h
  src/measurement.c
  src/measurement.h
  src/evidense.c
)

# Stuff only for WIN32
if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    target_sources(libevidense PRIVATE ${COMMOM_LIB}/evi_win.c)
    target_link_libraries(libevidense setupapi)
endif()

if (UNIX)
    set (CMAKE_C_FLAGS "-Wall")
    target_sources(libevidense PRIVATE ${COMMOM_LIB}/evi_unix.c)
    target_link_libraries(libevidense m)
    find_path(LIBUSB_INCLUDE_DIR NAMES libusb.h PATH_SUFFIXES "include" "libusb" "libusb-1.0")
    find_library(LIBUSB_LIBRARY NAMES usb PATH_SUFFIXES "lib" "lib32" "lib64")    
    target_link_libraries(libevidense usb-1.0)
endif()

set_target_properties(libevidense PROPERTIES PUBLIC_HEADER "src/channel.h;src/measurement.h;src/singlemeasurement.h;src/quadruple.h;src/evidense.h;${FW}/evidenseerror.h;${FW}/evidenseindex.h;${FW_COMMON}/commonerror.h;${FW_COMMON}/commonindex.h;${COMMOM_LIB}/evibase.h")

add_executable(evidense)
target_sources(evidense PRIVATE src/main.c
src/cmdmeasure.c
src/cmdbaseline.c
src/cmddata.c
src/cmdsave.c
src/cmdexport.c
src/cmdempty.c
src/json.c
src/cmdselftest.c
src/eviconfig.h
${COMMOM_CMD}/printerror.c
${COMMOM_CMD}/cmdcommand.c
${COMMOM_CMD}/cmdfwupdate.c
${COMMOM_CMD}/cmdget.c
${COMMOM_CMD}/cmdset.c)

FetchContent_Declare(
    cJSON
    GIT_REPOSITORY "https://github.com/DaveGamble/cJSON.git"
    GIT_TAG "v1.7.18"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
set(ENABLE_CJSON_TEST OFF CACHE BOOL "cJSON: Build with unit testing" FORCE)
FetchContent_MakeAvailable(cJSON)

message("cJSON_SOURCE_DIR    : ${cJSON_SOURCE_DIR}")

target_include_directories(evidense PRIVATE ${cJSON_SOURCE_DIR})
target_include_directories(evidense PRIVATE ${COMMOM_CMD})
target_include_directories(evidense PRIVATE ${COMMOM_LIB})
target_include_directories(evidense PRIVATE "${PROJECT_SOURCE_DIR}/src" "${FW}" "${FW_COMMON}")
target_link_libraries(evidense PRIVATE libevidense cjson)

install(TARGETS libevidense PUBLIC_HEADER)
install(TARGETS evidense)
