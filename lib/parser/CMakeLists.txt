add_library(parser STATIC
    ParserImpl.cpp
)

target_link_libraries(parser
    scanner
)
