cmake_minimum_required(VERSION 3.10)

project(VideoDSP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# La ubicación de OpenCV ya NO está hardcodeada acá.
# CMake va a buscar OpenCV instalado en el sistema.
# Si no lo encuentra automáticamente, pasar la ruta al invocar cmake, ej:
#   cmake -B build -S . -DOpenCV_DIR="C:/ruta/a/opencv/build"
find_package(OpenCV REQUIRED)

# frequencyFilters.cpp todavía está vacío (ver Roadmap en README).
# Cuando se implemente, agregarlo acá a la lista de fuentes.
add_executable(VideoDSP
    src/main.cpp
    src/camera.cpp
    src/videoProcessor.cpp
    src/spatialFilters.cpp
    src/metrics.cpp
)

target_include_directories(VideoDSP PRIVATE
    include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(VideoDSP PRIVATE ${OpenCV_LIBS})