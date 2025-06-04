cmake_minimum_required(VERSION 3.10)

project(CampusNavigation)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/Algorithm
    ${PROJECT_SOURCE_DIR}/LGraph
)

set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/Algorithm/Algorithm.cpp
    ${PROJECT_SOURCE_DIR}/LGraph/LGraph.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)

add_executable(CampusNavigation ${SRC_FILES})