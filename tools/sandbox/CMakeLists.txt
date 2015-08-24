add_custom_target(sandbox
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qore-llvm -bo ${CMAKE_CURRENT_BINARY_DIR}/test.bc ${CMAKE_CURRENT_SOURCE_DIR}/test.q
        COMMAND
            clang -S -emit-llvm -I ${CMAKE_SOURCE_DIR}/include -std=c++11 -o rt.bc ${CMAKE_SOURCE_DIR}/lib/runtime/runtime.cpp
        COMMAND
            clang -S -emit-llvm -I ${CMAKE_SOURCE_DIR}/include -std=c++11 -o main.bc ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        COMMAND
            opt -o maino.bc main.bc
        COMMAND
            opt -o rto.bc rt.bc
        COMMAND
            opt -o testo.bc test.bc
        COMMAND
            llvm-lto --exported-symbol=main -o joined.o maino.bc rto.bc testo.bc
        COMMAND
            g++ -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sandbox joined.o
        DEPENDS qore-llvm
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
