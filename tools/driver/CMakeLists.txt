find_package(LLVM REQUIRED CONFIG)

add_executable(qore-llvm
    driver.cpp
)

option(QORE_ENABLE_JIT "enable JIT" ON)

target_include_directories(qore-llvm PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(qore-llvm PRIVATE ${LLVM_DEFINITIONS})

if(QORE_ENABLE_JIT)
    llvm_map_components_to_libnames(llvm_libs core bitwriter mcjit native)
    target_compile_definitions(qore-llvm PRIVATE QORE_ENABLE_JIT)
else(QORE_ENABLE_JIT)
    llvm_map_components_to_libnames(llvm_libs core bitwriter)
endif(QORE_ENABLE_JIT)

target_link_libraries(qore-llvm
    common
    parser
    runtime
    ${llvm_libs}
)
