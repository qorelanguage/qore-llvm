add_library(cg STATIC
    CodeGen.cpp
)

target_link_libraries(cg
    common
)

setup_llvm(cg)
