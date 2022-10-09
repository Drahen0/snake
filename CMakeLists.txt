cmake_minimum_required(VERSION 3.13)
project(snake CXX)

# set(CMAKE_CXX_STANDARD 17)
include_directories(include)

add_executable(snake WIN32 src/main.cpp src/snake.cpp src/vec2d.cpp)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)


target_link_libraries(snake PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(snake PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)

add_custom_command(
    TARGET snake
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/OpenSans-Regular.ttf"
        "$<TARGET_FILE:snake>/../OpenSans-Regular.ttf"
)
