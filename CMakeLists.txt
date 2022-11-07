project(apps)

set(CPP_STANDARD 20)

add_executable(lsys
               LSysMain.cpp
               Debug.cpp
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
           Lexer.cpp
           Polygon.cpp
           Generator.cpp
           Options.cpp
           CommandLineOptions.cpp
           Standard_InitialiseFinalise.cpp
           CVSFileInfo.cpp
           Consts.cpp
                          )

    target_link_libraries(lsys
                          PRIVATE
                          m
                          stdc++
                          )
