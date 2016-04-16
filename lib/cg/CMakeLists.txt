add_library(cg STATIC
    CodeGen.cpp
    ScriptCompiler.cpp
)

target_link_libraries(cg
    common
)

setup_llvm(cg)
