cmake_minimum_required(VERSION 3.8)

project("WebDogfight")

add_subdirectory("src")
add_subdirectory("vendor")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "src")
