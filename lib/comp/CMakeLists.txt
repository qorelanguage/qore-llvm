add_library(comp STATIC
    DiagManager.cpp
    DiagRecord.cpp
    DirectiveProcessor.cpp
    Scanner.cpp
    Source.cpp
    SourceManager.cpp
    Token.cpp
)

target_link_libraries(comp
    common
)
