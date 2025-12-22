file(GLOB_RECURSE src CONFIGURE_DEPENDS "*.cpp" "*.hpp")

add_executable(WebDogfight ${src})

target_link_libraries(WebDogfight raylib)
