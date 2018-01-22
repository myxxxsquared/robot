
cmake_minimum_required(VERSION 3.5)

project(chatrobotclient)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/../common/CMake")

add_subdirectory(portaudio portaudio)

# find_package(Boost REQUIRED
#     COMPONENTS system)
find_package(Threads REQUIRED)
# find_package(Portaudio REQUIRED)
find_package(RT REQUIRED)
find_package(FFTW REQUIRED)


if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "-Wall -O0 -ggdb -std=gnu11 -DDEBUG")
    set(CMAKE_CXX_FLAGS "-Wall -O0 -ggdb -std=gnu++14 -DDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "-Wall -O0 -ggdb -Wl,--no-as-needed")
else()
    set(CMAKE_C_FLAGS "-flto -Wall -O3 -g -std=gnu11 -DNDEBUG")
    set(CMAKE_CXX_FLAGS "-flto -Wall -O3 -g -std=gnu++14 -DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "-flto -Wall -O3 -g -Wl,--no-as-needed")
endif()

include_directories(
    ${PORTAUDIO_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${PORTAUDIO_INCLUDE_DIRS}
    ${RT_INCLUDES}
    ${FFTW_INCLUDES}
    ${CMAKE_SOURCE_DIR}/../common
    ${CMAKE_SOURCE_DIR}/portaudio/include
    ${CMAKE_SOURCE_DIR}/xcorr)

aux_source_directory(${CMAKE_SOURCE_DIR} ROOT_SOURCES)
aux_source_directory(${CMAKE_SOURCE_DIR}/../common COMMON_SOURCES)
aux_source_directory(${CMAKE_SOURCE_DIR}/gpio GPIO_SOURCES)
aux_source_directory(${CMAKE_SOURCE_DIR}/webrtc/common_audio/vad VAD_SOURCES)
aux_source_directory(${CMAKE_SOURCE_DIR}/webrtc/common_audio/signal_processing SIG_SOURCES)
aux_source_directory(${CMAKE_SOURCE_DIR}/xcorr XCORR_SOURCES)

set(SERIAL_SOURCES
    serial/serial.cc
    serial/impl/unix.cc
    serial/impl/list_ports/list_ports_linux.cc)

add_executable(chatrobotclient
    ${ROOT_SOURCES}
    ${COMMON_SOURCES}
    ${SERIAL_SOURCES}
    ${GPIO_SOURCES}
    ${VAD_SOURCES}
    ${SIG_SOURCES}
    ${XCORR_SOURCES})

target_link_libraries(chatrobotclient
    # ${PORTAUDIO_LIBRARIES}
    portaudio
    # ${Boost_LIBRARIES}
    ${RT_LIBRARIES}
    Threads::Threads
    ${FFTW_LIBRARIES}
    )
