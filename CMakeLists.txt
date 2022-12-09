project(LSys)
cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
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

set(TARGET_LIB "lsys")
set(TARGET_APP "lsys-gen")

add_library(${TARGET_LIB}
        STATIC
        include/lsys/list.h
        include/lsys/polygon.h
        include/lsys/symbol_table.h
        src/actions.cpp
        src/consts.cpp
        src/expression.cpp
        src/generator.cpp
        src/generic_generator.cpp
        src/graphics_generator.cpp
        src/interpret.cpp
        src/l_sys_model.cpp
        src/lexer.cpp
        src/module.cpp
        src/name.cpp
        src/parsed_model.cpp
        src/parser.cpp
        src/production.cpp
        src/rand.cpp
        src/radiance_generator.cpp
        src/token.h
        src/turtle.cpp
        src/value.cpp
        src/vector.cpp
        )

add_library(LSys::lib ALIAS ${TARGET_LIB})

add_executable(${TARGET_APP}
        src/l_sys_main.cpp
        src/command_line_options.cpp
        src/options.cpp
        )

add_executable(LSys::gen ALIAS ${TARGET_APP})

target_include_directories(${TARGET_LIB}
        PUBLIC
        include/lsys
        )
target_include_directories(${TARGET_LIB}
        SYSTEM PRIVATE
        /usr/include
        )
target_include_directories(${TARGET_APP}
        PUBLIC
        include/lsys
        )
target_include_directories(${TARGET_APP}
        SYSTEM PRIVATE
        /usr/include
        )

target_link_libraries(${TARGET_APP}
        PRIVATE
        ${TARGET_LIB}
        pthread
        m
        stdc++
        )

set(POS_INDEP_CODE "ON")
set_target_properties(${TARGET_LIB}
        PROPERTIES
        POSITION_INDEPENDENT_CODE ${POS_INDEP_CODE}
        )

set_project_warnings(${TARGET_LIB} ${TARGET_APP})

set(MSVC_WARNINGS_OFF
        -Wno-date-time # TODO(glk) fix these
        )
set(CLANG_WARNINGS_OFF
        -Wno-extra
        -Wno-date-time
        -Wno-old-style-cast
        -Wno-conversion
        -Wno-sign-conversion
        -Wno-implicit-fallthrough
        -Wno-register
        -Wno-unused-function
        -Wno-write-strings
        )
set(GCC_WARNINGS_OFF
        ${CLANG_WARNINGS_OFF}
        -Wno-pedantic
        -Wno-useless-cast
        -Wno-effc++
        )

if (MSVC)
    set(SOME_WARNINGS_OFF "${MSVC_WARNINGS_OFF}")
elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(SOME_WARNINGS_OFF "${CLANG_WARNINGS_OFF}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(SOME_WARNINGS_OFF "${GCC_WARNINGS_OFF}")
endif ()
STRING(REPLACE ";" " " SOME_WARNINGS_OFF "${SOME_WARNINGS_OFF}")

set_source_files_properties(
        src/command_line_options.cpp
        src/lexer.cpp
        src/options.cpp
        src/parser.cpp
        PROPERTIES
        COMPILE_FLAGS "${SOME_WARNINGS_OFF}"
)

message(STATUS "LSys: CMAKE_CXX_COMPILER_ID = \"${CMAKE_CXX_COMPILER_ID}\".")
message(STATUS "LSys: CMAKE_CXX_COMPILER_VERSION = \"${CMAKE_CXX_COMPILER_VERSION}\".")
message(STATUS "LSys: CMAKE_BUILD_TYPE = \"${CMAKE_BUILD_TYPE}\".")
message(STATUS "LSys: WARNINGS_AS_ERRORS = \"${WARNINGS_AS_ERRORS}\".")
message(STATUS "LSys: SOME_WARNINGS_OFF = \"${SOME_WARNINGS_OFF}\".")
message(STATUS "LSys: CMAKE_MODULE_PATH = \"${CMAKE_MODULE_PATH}\".")
message(STATUS "LSys: CMAKE_SCRIPTS_PATH = \"${CMAKE_SCRIPTS_PATH}\".")
message(STATUS "LSys: CMAKE_GENERATOR = \"${CMAKE_GENERATOR}\".")
message(STATUS "LSys: GENERATOR_IS_MULTI_CONFIG = \"${GENERATOR_IS_MULTI_CONFIG}\".")
message(STATUS "LSys: CMAKE_RUNTIME_OUTPUT_DIRECTORY = \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\".")
message(STATUS "LSys: C++ standard = \"${CMAKE_CXX_STANDARD}\".")
message(STATUS "LSys: C++ standard required = \"${CMAKE_CXX_STANDARD_REQUIRED}\".")
message(STATUS "LSys: C++ extensions = \"${CMAKE_CXX_EXTENSIONS}\".")
