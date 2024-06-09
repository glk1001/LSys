cmake_minimum_required(VERSION 3.28)

project(LSys)

set(CMAKE_SCRIPTS_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_SCRIPTS_PATH}/CPM.cmake)
include(${CMAKE_SCRIPTS_PATH}/StandardProjectSettings.cmake)
include(${CMAKE_SCRIPTS_PATH}/PreventInSourceBuilds.cmake)
include(${CMAKE_SCRIPTS_PATH}/Cache.cmake)
include(${CMAKE_SCRIPTS_PATH}/CompilerWarnings.cmake)
include(${CMAKE_SCRIPTS_PATH}/Linker.cmake)
include(${CMAKE_SCRIPTS_PATH}/CheckMinimumToolRequirements.cmake)

set(TARGET_LIB "lsys")
set(TARGET_APP "lsys-gen")

include(${CMAKE_SCRIPTS_PATH}/GetLSysSources.cmake)
set(LSys_SOURCE_DIR "")
LSys_get_modules("${LSys_SOURCE_DIR}" LSys_modules)
LSys_get_source_files("${LSys_SOURCE_DIR}" LSys_source_files)
LSys_set_some_warnings_off("${LSys_SOURCE_DIR}")

add_library(${TARGET_LIB}
            STATIC
            ${LSys_source_files}
)

target_sources(${TARGET_LIB}
               PUBLIC
               FILE_SET CXX_MODULES FILES
               ${LSys_modules}
)

add_library(LSys::lib ALIAS ${TARGET_LIB})

add_executable(${TARGET_APP}
               main/l_sys_main.cpp
               main/command_line_options.cpp
               main/options.cpp
)

add_executable(LSys::gen ALIAS ${TARGET_APP})

target_include_directories(${TARGET_LIB}
                           PRIVATE
                           include/lsys
)
target_include_directories(${TARGET_LIB}
                           SYSTEM PRIVATE
                           /usr/include
)
target_include_directories(${TARGET_APP}
                           PRIVATE
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

include(ProjectOptions.cmake)
LSys_set_project_warnings(${LSys_WARNINGS_AS_ERRORS} ${TARGET_LIB})
LSys_set_project_warnings(${LSys_WARNINGS_AS_ERRORS} ${TARGET_APP})

set(MSVC_WARNINGS_OFF
    /wd4005
    /wd4201
    /wd4996
    /w
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
    -Wno-switch-default
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
        src/name.cpp
        src/options.cpp
        src/parser.cpp
        src/value.cpp
        src/vector.cpp
        PROPERTIES
        COMPILE_FLAGS "${SOME_WARNINGS_OFF}"
)

message(STATUS "LSys: CMAKE_CXX_COMPILER_ID          = \"${CMAKE_CXX_COMPILER_ID}\".")
message(STATUS "LSys: CMAKE_CXX_COMPILER_VERSION     = \"${CMAKE_CXX_COMPILER_VERSION}\".")
message(STATUS "LSys: CMAKE_BUILD_TYPE               = \"${CMAKE_BUILD_TYPE}\".")
message(STATUS "LSys: WARNINGS_AS_ERRORS             = \"${WARNINGS_AS_ERRORS}\".")
message(STATUS "LSys: SOME_WARNINGS_OFF              = \"${SOME_WARNINGS_OFF}\".")
message(STATUS "LSys: CMAKE_MODULE_PATH              = \"${CMAKE_MODULE_PATH}\".")
message(STATUS "LSys: CMAKE_SCRIPTS_PATH             = \"${CMAKE_SCRIPTS_PATH}\".")
message(STATUS "LSys: CMAKE_GENERATOR                = \"${CMAKE_GENERATOR}\".")
message(STATUS "LSys: GENERATOR_IS_MULTI_CONFIG      = \"${GENERATOR_IS_MULTI_CONFIG}\".")
message(STATUS "LSys: CMAKE_RUNTIME_OUTPUT_DIRECTORY = \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\".")
message(STATUS "LSys: C++ standard                   = \"${CMAKE_CXX_STANDARD}\".")
message(STATUS "LSys: C++ standard required          = \"${CMAKE_CXX_STANDARD_REQUIRED}\".")
message(STATUS "LSys: C++ extensions                 = \"${CMAKE_CXX_EXTENSIONS}\".")
