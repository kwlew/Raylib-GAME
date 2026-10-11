cmake_minimum_required(VERSION 3.24)

project(
    Raylib_GAME
    VERSION 0.1.0
    DESCRIPTION "A raylib game."
    LANGUAGES C CXX
)

option(GAME_ENABLE_CLANG_TIDY "Run clang-tidy while compiling Game" OFF)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Keep the dependency reproducible by tracking a released raylib tag, not its
# development branch.
set(BUILD_EXAMPLES OFF CACHE BOOL "Build raylib examples" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "Build raylib games" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

# Every .cpp under src/ is part of the game. CONFIGURE_DEPENDS re-checks the
# glob on each build, so new files are picked up without editing this list.
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(Raylib_GAME ${GAME_SOURCES}
    src/game/kernel/input.cpp
    src/game/entities/zombie.cpp)

target_compile_features(Raylib_GAME PRIVATE cxx_std_23)
set_target_properties(
    Raylib_GAME
    PROPERTIES
        CXX_EXTENSIONS OFF
)

# Single source of truth for the game version: the VERSION in project() above.
# Include it with #include "game/version.hpp".
configure_file(
    cmake/version.hpp.in
    "${CMAKE_CURRENT_BINARY_DIR}/generated/include/game/version.hpp"
    @ONLY
)

target_include_directories(
    Raylib_GAME
    PRIVATE
        include
        "${CMAKE_CURRENT_BINARY_DIR}/generated/include"
)
target_link_libraries(Raylib_GAME PRIVATE raylib)

# GetProcessMemoryInfo (os/memory.cpp) lives in psapi on Windows.
if(WIN32)
    target_link_libraries(Raylib_GAME PRIVATE psapi)
endif()

# add_custom_command(
#     TARGET Raylib_GAME
#     POST_BUILD
#     COMMAND
#         ${CMAKE_COMMAND} -E copy_directory
#         "${CMAKE_CURRENT_SOURCE_DIR}/src/assets"
#         "$<TARGET_FILE_DIR:Raylib_GAME>/assets"
#     COMMENT "Copying Game assets"
# )

if(GAME_ENABLE_CLANG_TIDY)
    find_program(
        GAME_CLANG_TIDY_EXECUTABLE
        NAMES clang-tidy clang-tidy-22 clang-tidy-21 clang-tidy-20 clang-tidy-19 clang-tidy-18
        REQUIRED
    )
    set_target_properties(
        Raylib_GAME
        PROPERTIES
            CXX_CLANG_TIDY
                "${GAME_CLANG_TIDY_EXECUTABLE};--config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy"
    )
endif()

if(MSVC)
    target_compile_options(Raylib_GAME PRIVATE /W4 /permissive-)
else()
    target_compile_options(Raylib_GAME PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()
