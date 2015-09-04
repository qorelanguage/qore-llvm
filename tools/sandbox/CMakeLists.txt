add_custom_target(sandbox
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qore-llvm -lo ${CMAKE_CURRENT_BINARY_DIR}/test.ll ${CMAKE_CURRENT_SOURCE_DIR}/test.q
        COMMAND
            clang -S -emit-llvm -I ${CMAKE_SOURCE_DIR}/include -std=c++11 -g -o rt.ll ${CMAKE_SOURCE_DIR}/lib/runtime/runtime.cpp
        COMMAND
            clang -S -emit-llvm -I ${CMAKE_SOURCE_DIR}/include -std=c++11 -g -o main.ll ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        COMMAND
            llvm-as -o test.bc test.ll
        COMMAND
            llvm-as -o main.bc main.ll
        COMMAND
            llvm-as -o rt.bc rt.ll
        COMMAND
            llvm-lto --exported-symbol=main -o joined.o main.bc rt.bc test.bc
        COMMAND
            g++ -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sandbox -g joined.o
        DEPENDS qore-llvm
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
