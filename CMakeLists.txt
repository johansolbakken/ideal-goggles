cmake_minimum_required(VERSION 3.3)

project(Raytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

file(GLOB helloworld_SRC
     "src/*.h"
     "src/*.cpp"
     "src/*.hpp"
)

add_subdirectory(vendor/glm)

add_executable(raytracer ${helloworld_SRC})
target_include_directories(raytracer PUBLIC src vendor/glm)