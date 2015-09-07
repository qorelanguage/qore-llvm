add_library(parser STATIC
    ParserImpl.cpp
)

target_link_libraries(parser
    scanner
)

target_compile_definitions(parser PRIVATE QORE_LOG_COMPONENT="PARSER")
