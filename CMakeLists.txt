project(apps)

set(CPP_STANDARD 20)

add_executable(lsys
        LSysMain.cpp
        debug.cpp
        Expression.cpp
        GenericGenerator.cpp
        Interpret.cpp
        List.cpp
        LSysModel.cpp
        Module.cpp
        Name.cpp
        Parser.cpp
        Production.cpp
        SymbolTable.cpp
        Turtle.cpp
        Value.cpp
        Vector.cpp
        Actions.cpp
#        Lexer.cpp
        Polygon.cpp
        Generator.cpp
        Options.cpp
        CommandLineOptions.cpp
        Consts.cpp
        token.h
        )

target_link_libraries(lsys
        PRIVATE
        m
        stdc++
        )
