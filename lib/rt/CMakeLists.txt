add_library(rt STATIC
    Func.cpp
    Meta.cpp
)

target_link_libraries(rt
    common
)
