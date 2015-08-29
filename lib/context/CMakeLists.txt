add_library(context STATIC
    SourceManager.cpp
)

target_link_libraries(context
    common
)

target_compile_definitions(context PRIVATE QORE_LOG_COMPONENT="CTX")
