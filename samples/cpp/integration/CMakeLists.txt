cmake_minimum_required(VERSION 3.10)
project(pplnn-integration)

set(pplnn_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../pplnn-build/install/lib/cmake/ppl)
find_package(pplnn REQUIRED)

add_executable(pplnn-integration main.cc)

target_include_directories(pplnn-integration PRIVATE ${PPLNN_INCLUDE_DIRS})
target_link_libraries(pplnn-integration PRIVATE ${PPLNN_LIBRARIES})
