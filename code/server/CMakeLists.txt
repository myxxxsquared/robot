
cmake_minimum_required(VERSION 3.5)

project(chatrobot)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/CMake")

option(USE_STDIN "using stdin as input" OFF)
option(USE_STDOUT "using stdout as output" OFF)

if(MSVC)
    set(XF_LIBRARY_DIRS
        "${CMAKE_SOURCE_DIR}/3rdparty/xf/windows_x64/libs"
        "${CMAKE_SOURCE_DIR}/3rdparty/xf/windows_x86/libs")
    set(CHATROBOT_SUPPORTED ON)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(UNIX)
    if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
        set(CMAKE_CXX_FLAGS "-Wall -O0 -ggdb")
        set(CMAKE_EXE_LINKER_FLAGS "")
    else()
        set(CMAKE_CXX_FLAGS "-flto -Wall -O3 -g")
        set(CMAKE_EXE_LINKER_FLAGS "-flto")
    endif()

    if(CMAKE_LIBRARY_ARCHITECTURE MATCHES "arm-linux-gnueabihf")
        set(CHATROBOT_SUPPORTED ON)
        set(XF_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/xf/linux_arm/libs")
    elseif(CMAKE_LIBRARY_ARCHITECTURE MATCHES "x86-linux-gnu")
        set(CHATROBOT_SUPPORTED ON)
        set(XF_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/xf/linux_x86/libs")
    elseif(CMAKE_LIBRARY_ARCHITECTURE MATCHES "x86_64-linux-gnu")
        set(XF_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/xf/linux_x64/libs")
        set(CHATROBOT_SUPPORTED ON)
    endif()
endif()

if(NOT CHATROBOT_SUPPORTED)
    message(FATAL_ERROR "the platform is not supported." )
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

find_package(Portaudio REQUIRED)
find_package(Threads REQUIRED)
find_package(CURLpp REQUIRED)

include(cotire)

configure_file(
    "config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h")

include_directories(
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}"
    "${CMAKE_SOURCE_DIR}/3rdparty/xf/include"
    "${CMAKE_SOURCE_DIR}/3rdparty/json"
    "${CMAKE_SOURCE_DIR}/3rdparty/curlpp/include"
    ${PORTAUDIO_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS})

link_directories(
    ${XF_LIBRARY_DIRS})

add_executable(chatrobot
    main.cpp
    init_audio.cpp
    record.cpp
    play.cpp
    dialog.cpp
    recognize.cpp
    generate.cpp
    init_threads.cpp
    init_xf.cpp
    3rdparty/curlpp/src/curlpp/cURLpp.cpp
    3rdparty/curlpp/src/curlpp/Easy.cpp
    3rdparty/curlpp/src/curlpp/Exception.cpp
    3rdparty/curlpp/src/curlpp/Form.cpp
    3rdparty/curlpp/src/curlpp/Info.cpp
    3rdparty/curlpp/src/curlpp/Multi.cpp
    3rdparty/curlpp/src/curlpp/OptionBase.cpp
    3rdparty/curlpp/src/curlpp/Options.cpp
    3rdparty/curlpp/src/curlpp/internal/CurlHandle.cpp
    3rdparty/curlpp/src/curlpp/internal/OptionList.cpp
    3rdparty/curlpp/src/curlpp/internal/OptionSetter.cpp
    3rdparty/curlpp/src/curlpp/internal/SList.cpp)

target_link_libraries(chatrobot
    ${PORTAUDIO_LIBRARIES}
    ${CURL_LIBRARIES}
    Threads::Threads
    "msc")

set(CHATROBOT_SUPPORTED OFF)

cotire(chatrobot)
