cmake_minimum_required(VERSION 3.29)
project(KNN_HandWriting_Recognize)

include_directories(${PROJECT_SOURCE_DIR}/include)

link_directories(${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 20)

add_executable(KNN_HandWriting_Recognize main.cpp)
