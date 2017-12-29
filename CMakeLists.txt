cmake_minimum_required(VERSION 2.8)

project(Kitanai)

file(GLOB_RECURSE PG_KTN "include/*.hpp")
file(GLOB_RECURSE HD_KTN "src/*.cpp")

include_directories("include/")

add_executable(Kitanai ${HD_KTN} ${PG_KITANAI})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)