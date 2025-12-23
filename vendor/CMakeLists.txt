
if(EMSCRIPTEN)
    add_subdirectory("glm")
else()
    add_subdirectory("glfw")
    add_subdirectory("glm")
endif()
