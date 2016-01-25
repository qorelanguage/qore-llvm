add_library(comp STATIC
    DiagManager.cpp
    DiagRecord.cpp
)

target_link_libraries(comp
    common
)
