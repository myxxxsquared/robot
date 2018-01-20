
cmake_minimum_required(VERSION 3.5)

project(chatrobot)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/CMake")

find_package(Boost REQUIRED
    COMPONENTS system)
find_package(Threads REQUIRED)
find_package(Portaudio REQUIRED)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "-Wall -O0 -ggdb -std=gnu11 -DDEBUG")
    set(CMAKE_CXX_FLAGS "-Wall -O0 -ggdb -std=gnu++14 -DDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "")
else()
    set(CMAKE_C_FLAGS "-flto -Wall -O3 -g -std=gnu11 -DNDEBUG")
    set(CMAKE_CXX_FLAGS "-flto -Wall -O3 -g -std=gnu++14 -DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "-flto")
endif()

include_directories(
    ${PORTAUDIO_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${PORTAUDIO_INCLUDE_DIRS})

aux_source_directory(. ROOT_SOURCES)
aux_source_directory(gpio GPIO_SOURCES)
aux_source_directory(webrtc/common_audio/vad VAD_SOURCES)
aux_source_directory(webrtc/common_audio/signal_processing SIG_SOURCES)

add_executable(robotaudio
    ${ROOT_SOURCES}
    ${GPIO_SOURCES}
    ${VAD_SOURCES}
    ${SIG_SOURCES})

target_link_libraries(robotaudio
    ${PORTAUDIO_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PORTAUDIO_LIBRARIES}
    Threads::Threads)
