cmake_minimum_required(VERSION 3.14)
project(ColonySimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# raylib
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/raylib ${CMAKE_BINARY_DIR}/_deps/raylib-build SYSTEM)

# RayUI
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/RayUI ${CMAKE_BINARY_DIR}/_deps/RayUI-build SYSTEM)

# JsonFormat
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/JsonFormat ${CMAKE_BINARY_DIR}/_deps/JsonFormat-build SYSTEM)

if (LUA_BINDINGS)
    # lua
    file(GLOB LUA_SOURCES thirdparty/lua/*.c)
    list(REMOVE_ITEM LUA_SOURCES thirdparty/lua/lua.c thirdparty/lua/luac.c)
    add_library(lua STATIC ${LUA_SOURCES})
    target_include_directories(lua PUBLIC thirdparty/lua)
    if(WIN32)
        target_compile_definitions(lua PRIVATE LUA_USE_WINDOWS)
    elseif(APPLE)
        target_compile_definitions(lua PRIVATE LUA_USE_MACOSX)
    elseif(UNIX)
        target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
    endif()

    # sol2
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/sol2 ${CMAKE_BINARY_DIR}/_deps/sol2-build SYSTEM)

    # lua-bindings
    file(GLOB_RECURSE LUA_BINDINGS_SOURCES tools/*.cpp)
    add_library(lua-bindings STATIC ${LUA_BINDINGS_SOURCES})
    target_include_directories(lua-bindings PUBLIC include/ tools/)
    target_compile_options(lua-bindings PRIVATE -O0)
    target_link_libraries(lua-bindings raylib RayUI JsonFormat lua sol2)
    target_precompile_headers(lua-bindings PRIVATE tools/Includes.hpp)
    if(WIN32 OR MINGW)
        target_compile_options(lua-bindings PRIVATE "-Wa,-mbig-obj")
    endif()
endif() # LUA_BINDINGS

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include/)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib RayUI JsonFormat)
if (LUA_BINDINGS)
    target_link_libraries(${PROJECT_NAME} PRIVATE lua sol2 lua-bindings)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LUA_BINDINGS)
endif()

# Disable the console on a Windows release build
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
endif()
