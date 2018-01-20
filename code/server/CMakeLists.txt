
cmake_minimum_required(VERSION 3.5)

project(chatrobotserver)

add_subdirectory(gazr gazr)
add_subdirectory(curlpp curlpp)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/../common/CMake")

find_package(Threads REQUIRED)
# find_package(CURLpp REQUIRED)

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
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../common
    curlpp/include
    gazr
    json
    xf/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/xf/libs)


aux_source_directory(${CMAKE_SOURCE_DIR} ROOT_SOURCES)
aux_source_directory(${CMAKE_SOURCE_DIR}/../common COMMON_SOURCES)

add_executable(chatrobotserver
    ${ROOT_SOURCES}
    ${COMMON_SOURCES})

target_link_libraries(chatrobotserver
    Threads::Threads
    gazr
    curlpp
    "msc")
