add_library(scanner STATIC
    ScannerImpl.cpp
)

target_link_libraries(scanner
    common
)

target_compile_definitions(scanner PRIVATE QORE_LOG_COMPONENT="SCANNER")
