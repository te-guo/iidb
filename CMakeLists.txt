cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(Neru)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/third_party/filesystem)

file(GLOB_RECURSE TEST_FILESYSTEM_SRC ${PROJECT_SOURCE_DIR}/third_party/filesystem/*.cpp)
add_executable(test_filesystem ${TEST_FILESYSTEM_SRC} ${PARSER_SRC})

file(GLOB_RECURSE UTILS_SRC ${PROJECT_SOURCE_DIR}/third_party/filesystem/utils/*.cpp)

file(GLOB_RECURSE NERU_SRC ${PROJECT_SOURCE_DIR}/src/*.cc)
add_executable(neru ${NERU_SRC} ${UTILS_SRC} ${PARSER_SRC})