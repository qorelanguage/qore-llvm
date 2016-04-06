add_library(cg STATIC
    CodeGen.cpp
    ExpressionCompiler.cpp
    LValueCompiler.cpp
    ScriptCompiler.cpp
    StatementCompiler.cpp
)

target_link_libraries(cg
    common
    ir
)

setup_llvm(cg)
