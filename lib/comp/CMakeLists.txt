add_library(comp STATIC
    DiagManager.cpp
    DiagRecord.cpp
    Scanner.cpp
    Source.cpp
    Token.cpp
)

target_link_libraries(comp
    common
)
