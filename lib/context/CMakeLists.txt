add_library(context STATIC
    DiagManager.cpp
    SourceManager.cpp
)

target_link_libraries(context
    common
)

target_compile_definitions(context PRIVATE QORE_LOG_COMPONENT="CTX")
