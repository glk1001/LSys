project(LSys)
cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD 20)
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
        src/Actions.cpp
        src/CommandLineOptions.cpp
        src/Consts.cpp
        src/Expression.cpp
        src/GenericGenerator.cpp
        src/IGenerator.cpp
        src/Interpret.cpp
        src/Lexer.cpp
        src/List.h
        src/LSysMain.cpp
        src/LSysModel.cpp
        src/Module.cpp
        src/Name.cpp
        src/Options.cpp
        src/Parser.cpp
        src/Polygon.h
        src/Production.cpp
        src/SymbolTable.cpp
        src/token.h
        src/Turtle.cpp
        src/Value.cpp
        src/Vector.cpp
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
message(STATUS "LSys: C++ standard = \"${CXX_STANDARD}\".")
message(STATUS "LSys: C++ standard required = \"${CMAKE_CXX_STANDARD_REQUIRED}\".")
message(STATUS "LSys: C++ extensions = \"${CMAKE_CXX_EXTENSIONS}\".")
