add_library(scanner STATIC
    ScannerImpl.cpp
    Token.cpp
)

target_link_libraries(scanner
    context
)

target_compile_definitions(scanner PRIVATE QORE_LOG_COMPONENT="SCANNER")
