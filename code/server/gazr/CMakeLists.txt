cmake_minimum_required(VERSION 2.8.3)

project(gazr)

add_subdirectory(dlib dlib_build)

SET( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC" )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fPIC" )

find_package(OpenCV COMPONENTS core imgproc calib3d highgui REQUIRED)
# find_package(Boost COMPONENTS program_options system thread REQUIRED)

if(OPENCV3)
    find_package(OpenCV COMPONENTS core imgproc calib3d highgui imgcodecs videoio REQUIRED)
else()
    find_package(OpenCV COMPONENTS core imgproc calib3d highgui REQUIRED)
endif()

# message(STATUS "OpenCV version: ${OpenCV_VERSION}")
if(${OpenCV_VERSION} VERSION_GREATER 2.9.0)
    set(OPENCV3 TRUE)
    add_definitions(-DOPENCV3)
endif()

# if(DEBUG_OUTPUT)
#     add_definitions(-DHEAD_POSE_ESTIMATION_DEBUG)
# endif()

include_directories(${OpenCV_INCLUDE_DIRS})

add_library(gazr SHARED head_pose_estimation.cpp)
target_link_libraries(gazr dlib::dlib ${OpenCV_LIBRARIES})

# ${OpenCV_LIBRARIES} ${Boost_LIBRARIES}

