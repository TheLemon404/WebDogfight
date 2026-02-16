cmake_minimum_required(VERSION 3.31)
project("WebDogfight")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

if(EMSCRIPTEN)
    message(STATUS "Building for WebGL with Emscripten")
    # Delete the previous .data file so that it automatically updates
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_WEBGL2=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s MIN_WEBGL_VERSION=2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s MAX_WEBGL_VERSION=2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

    # Use custom html file
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --shell-file ${CMAKE_SOURCE_DIR}/shell_minimal.html")

    # Add uniforms for transformations
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_WEBGL2=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s MIN_WEBGL_VERSION=2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s MAX_WEBGL_VERSION=2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fexceptions")

    # compile resources to .data file
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/resources@/resources")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s NO_DISABLE_EXCEPTION_CATCHING")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s NO_DISABLE_EXCEPTION_CATCHING")

    # Only add GLM for Emscripten (skip GLFW since it's provided)
    add_subdirectory("vendor/glm")
    add_subdirectory("vendor/json")
    add_subdirectory("vendor/freetype")

    file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
    add_executable(WebDogfight ${src}
        "vendor/stb_image/stb_image.h"
        "vendor/librg/librg.h"
        "vendor/miniaudio/miniaudio.h"
    )

    target_include_directories(WebDogfight PUBLIC "vendor/stb_image" "vendor/miniaudio" "vendor/freetype/include")
    target_link_libraries(WebDogfight glm nlohmann_json freetype)
else()
    message(STATUS "Building for GLFW")

    add_subdirectory("vendor/glfw")
    add_subdirectory("vendor/glm")
    add_subdirectory("vendor/json")
    add_subdirectory("vendor/freetype")

    file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
    add_executable(WebDogfight ${src}
        "vendor/glad/src/glad.c"
        "vendor/stb_image/stb_image.h"
        "vendor/librg/librg.h"
        "vendor/miniaudio/miniaudio.h"
    )

    target_include_directories(WebDogfight PUBLIC "vendor/glad/include" "vendor/stb_image" "vendor/miniaudio" "vendor/freetype/include")
    target_link_libraries(WebDogfight glfw glm nlohmann_json freetype)

    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()
