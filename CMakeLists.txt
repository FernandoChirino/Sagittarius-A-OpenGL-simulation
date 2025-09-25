cmake_minimum_required(VERSION 3.12.0)
project(Sagittarius_A VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(OpenGL REQUIRED)

add_executable(Sagittarius_A
    src/config.h
    src/main.cpp 
    src/glad.c
    src/view/shader.cpp
    src/controller/app.cpp
    src/BlackHole.cpp
    src/Ray.cpp
)

target_include_directories(Sagittarius_A
    PRIVATE
    dependencies 
)

target_link_libraries(Sagittarius_A
    "${CMAKE_SOURCE_DIR}/dependencies/GLFW/lib-mingw-w64/libglfw3.a"
    opengl32
)