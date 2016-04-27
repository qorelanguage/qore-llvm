add_library(core STATIC
    BinaryOperator.cpp
    Conversion.cpp
    Data.cpp
)

target_link_libraries(core
    common
)

add_subdirectory(cg)
add_subdirectory(common)
add_subdirectory(comp)
add_subdirectory(in)
add_subdirectory(rt)
