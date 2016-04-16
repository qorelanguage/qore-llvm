add_library(in STATIC
    Interpreter.cpp
)

target_link_libraries(in
    rt
)
