add_library(cg STATIC
    CodeGen.cpp
    ScriptCompiler.cpp
)

target_link_libraries(cg
    common
    ir
)

setup_llvm(cg)
