cmake_minimum_required(VERSION 3.13.4)
set(CMAKE_CXX_STANDARD 17)
project (GameOfLife)

file(GLOB SOURCES "src/*.cpp" "src.*.c" "src/*.hpp" "src/*.h")

add_executable(GameOfLife ${SOURCES})

find_package(glad CONFIG REQUIRED)

find_package(SFML 2 COMPONENTS system window REQUIRED)

find_package(glm REQUIRED)

find_package(OpenGL REQUIRED)

target_link_libraries(GameOfLife PRIVATE glm::glm glad::glad sfml-window sfml-system ${OPENGL_LIBRARY}) 

