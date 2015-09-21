add_library(qil STATIC
    Instruction.cpp
    StringLiteral.cpp
    Variable.cpp
)

target_link_libraries(qil
    context
)

target_compile_definitions(qil PRIVATE QORE_LOG_COMPONENT="QIL")
