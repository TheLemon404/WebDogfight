cmake_minimum_required(VERSION 3.8)

project("WebDogfight")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory("vendor")

file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
add_executable(WebDogfight ${src} "vendor/glad/src/glad.c")

target_include_directories(WebDogfight PUBLIC "vendor/glad/include")

target_link_libraries(WebDogfight glfw)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
