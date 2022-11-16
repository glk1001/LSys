project(LSys)
cmake_minimum_required(VERSION 3.24)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_SCRIPTS_PATH "cmake")

include(${CMAKE_SCRIPTS_PATH}/StandardProjectSettings.cmake)
include(${CMAKE_SCRIPTS_PATH}/PreventInSourceBuilds.cmake)
include(${CMAKE_SCRIPTS_PATH}/Cache.cmake)
include(${CMAKE_SCRIPTS_PATH}/Linker.cmake)
include(${CMAKE_SCRIPTS_PATH}/CompilerWarnings.cmake)
include(${CMAKE_SCRIPTS_PATH}/Sanitizers.cmake)
include(${CMAKE_SCRIPTS_PATH}/StaticAnalyzers.cmake)
include(${CMAKE_SCRIPTS_PATH}/CheckMinimumToolRequirements.cmake)

set(TARGET_NAME "LSys")

add_executable(${TARGET_NAME}
        src/actions.cpp
        src/command_line_options.cpp
        src/consts.cpp
        src/expression.cpp
        src/generator.cpp
        src/generic_generator.cpp
        src/graphics_generator.cpp
        src/interpret.cpp
        src/l_sys_main.cpp
        src/l_sys_model.cpp
        src/lexer.cpp
        src/list.h
        src/module.cpp
        src/name.cpp
        src/options.cpp
        src/parsed_model.cpp
        src/parser.cpp
        src/polygon.h
        src/production.cpp
        src/radiance_generator.cpp
        src/symbol_table.h
        src/token.h
        src/turtle.cpp
        src/value.cpp
        src/vector.cpp
        )

target_include_directories(${TARGET_NAME}
        SYSTEM PRIVATE
        /usr/include
        )

target_link_libraries(${TARGET_NAME}
        PRIVATE
        m
        stdc++
        )

set_project_warnings(${TARGET_NAME})

message(STATUS "LSys: CMAKE_CXX_COMPILER_ID = \"${CMAKE_CXX_COMPILER_ID}\".")
message(STATUS "LSys: CMAKE_CXX_COMPILER_VERSION = \"${CMAKE_CXX_COMPILER_VERSION}\".")
message(STATUS "LSys: CMAKE_BUILD_TYPE = \"${CMAKE_BUILD_TYPE}\".")
message(STATUS "LSys: WARNINGS_AS_ERRORS = \"${WARNINGS_AS_ERRORS}\".")
message(STATUS "LSys: CMAKE_MODULE_PATH = \"${CMAKE_MODULE_PATH}\".")
message(STATUS "LSys: CMAKE_SCRIPTS_PATH = \"${CMAKE_SCRIPTS_PATH}\".")
message(STATUS "LSys: CMAKE_GENERATOR = \"${CMAKE_GENERATOR}\".")
message(STATUS "LSys: GENERATOR_IS_MULTI_CONFIG = \"${GENERATOR_IS_MULTI_CONFIG}\".")
message(STATUS "LSys: CMAKE_RUNTIME_OUTPUT_DIRECTORY = \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\".")
message(STATUS "LSys: C++ standard = \"${CMAKE_CXX_STANDARD}\".")
message(STATUS "LSys: C++ standard required = \"${CMAKE_CXX_STANDARD_REQUIRED}\".")
message(STATUS "LSys: C++ extensions = \"${CMAKE_CXX_EXTENSIONS}\".")
