add_library(comp STATIC
    DiagManager.cpp
    DiagRecord.cpp
    Source.cpp
    Token.cpp
)

target_link_libraries(comp
    common
)
