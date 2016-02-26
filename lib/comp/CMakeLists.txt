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
    Source.cpp
    SourceManager.cpp
    Token.cpp
    semantic/Analyzer.cpp
    semantic/Namespace.cpp
    semantic/TypeRegistry.cpp
)

target_link_libraries(comp
    common
)
