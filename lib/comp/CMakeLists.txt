add_library(comp STATIC
    DiagManager.cpp
    DiagRecord.cpp
    DirectiveProcessor.cpp
    Parser-class.cpp
    Parser-expression.cpp
    Parser-namespace.cpp
    Parser-recovery.cpp
    Parser-routine.cpp
    Parser-statement.cpp
    Scanner.cpp
    SourceManager.cpp
    Token.cpp
    sem/Analyzer.cpp
    sem/Builder.cpp
    sem/Core.cpp
    sem/FunctionGroupInfo.cpp
    sem/FunctionScope.cpp
    sem/GlobalVariableInfo.cpp
    sem/NamespaceScope.cpp
)

target_link_libraries(comp
    core
)
