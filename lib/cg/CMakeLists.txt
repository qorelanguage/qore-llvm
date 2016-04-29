add_library(cg STATIC
    CodeGen.cpp
)

target_link_libraries(cg
    core
)

setup_llvm(cg)
