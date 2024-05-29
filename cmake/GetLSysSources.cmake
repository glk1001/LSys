function(LSys_get_modules LSys_root_dir module_files)
    set(LSys_modules
        ${LSys_root_dir}include/lsys/actions.cppm
        ${LSys_root_dir}include/lsys/consts.cppm
        ${LSys_root_dir}include/lsys/expression.cppm
        ${LSys_root_dir}include/lsys/generator.cppm
        ${LSys_root_dir}include/lsys/generic_generator.cppm
        ${LSys_root_dir}include/lsys/graphics_generator.cppm
        ${LSys_root_dir}include/lsys/interpret.cppm
        ${LSys_root_dir}include/lsys/l_sys_model.cppm
        ${LSys_root_dir}include/lsys/list.cppm
        ${LSys_root_dir}include/lsys/module.cppm
        ${LSys_root_dir}include/lsys/name.cppm
        ${LSys_root_dir}include/lsys/parsed_model.cppm
        ${LSys_root_dir}include/lsys/polygon.cppm
        ${LSys_root_dir}include/lsys/production.cppm
        ${LSys_root_dir}include/lsys/radiance_generator.cppm
        ${LSys_root_dir}include/lsys/rand.cppm
        ${LSys_root_dir}include/lsys/symbol_table.cppm
        ${LSys_root_dir}include/lsys/turtle.cppm
        ${LSys_root_dir}include/lsys/value.cppm
        ${LSys_root_dir}include/lsys/vector.cppm
    )

    SET(${module_files} ${LSys_modules} PARENT_SCOPE)
endfunction()

function(LSys_get_source_files LSys_root_dir source_files)
    set(LSys_source_files
        ${LSys_root_dir}include/lsys/debug.h
        ${LSys_root_dir}include/lsys/parser.h
        ${LSys_root_dir}src/actions.cpp
        ${LSys_root_dir}src/consts.cpp
        ${LSys_root_dir}src/expression.cpp
        ${LSys_root_dir}src/generator.cpp
        ${LSys_root_dir}src/generic_generator.cpp
        ${LSys_root_dir}src/graphics_generator.cpp
        ${LSys_root_dir}src/interpret.cpp
        ${LSys_root_dir}src/l_sys_model.cpp
        ${LSys_root_dir}src/lexer.cpp
        ${LSys_root_dir}src/module.cpp
        ${LSys_root_dir}src/name.cpp
        ${LSys_root_dir}src/parsed_model.h
        ${LSys_root_dir}src/parsed_model.cpp
        ${LSys_root_dir}src/parser.cpp
        ${LSys_root_dir}src/production.cpp
        ${LSys_root_dir}src/rand.cpp
        ${LSys_root_dir}src/radiance_generator.cpp
        ${LSys_root_dir}src/token.h
        ${LSys_root_dir}src/turtle.cpp
        ${LSys_root_dir}src/value.cpp
        ${LSys_root_dir}src/vector.cpp
    )

    SET(${source_files} ${LSys_source_files} PARENT_SCOPE)
endfunction()

function(LSys_set_some_warnings_off LSys_root_dir)
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
            ${LSys_root_dir}src/command_line_options.cpp
            ${LSys_root_dir}src/lexer.cpp
            ${LSys_root_dir}src/name.cpp
            ${LSys_root_dir}src/options.cpp
            ${LSys_root_dir}src/parser.cpp
            ${LSys_root_dir}src/value.cpp
            ${LSys_root_dir}src/vector.cpp
            PROPERTIES
            COMPILE_FLAGS "${SOME_WARNINGS_OFF}"
    )
endfunction()
