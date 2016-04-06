add_library(in STATIC
    ExpressionEvaluator.cpp
    Interpreter.cpp
    LValueEvaluator.cpp
    StatementExecutor.cpp
)

target_link_libraries(in
    rt
    ir
)
