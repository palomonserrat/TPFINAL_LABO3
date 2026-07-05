cmake_minimum_required(VERSION 3.10)

project(VideoDSP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "C:/Users/palom/Downloads/opencv/build/x64/vc16/lib")

find_package(OpenCV REQUIRED)

add_executable(VideoDSP
    src/main.cpp
    src/camera.cpp
    src/videoProcessor.cpp
)

target_include_directories(VideoDSP PRIVATE
    include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(VideoDSP PRIVATE ${OpenCV_LIBS})