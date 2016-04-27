add_library(rt STATIC
    BinaryOperators.cpp
    Conversions.cpp
)

target_link_libraries(rt
    common
)
