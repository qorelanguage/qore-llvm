add_library(comp STATIC
    DiagManager.cpp
    DiagRecord.cpp
    Token.cpp
)

target_link_libraries(comp
    common
)
