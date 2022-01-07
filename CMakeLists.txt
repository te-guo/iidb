cmake_minimum_required(VERSION 3.16)
project(Neru)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE NERU_SRC ${PROJECT_SOURCE_DIR}/src/*.cc)
add_executable(neru ${NERU_SRC})

find_package(Threads)
target_link_libraries(neru ${CMAKE_THREAD_LIBS_INIT})
