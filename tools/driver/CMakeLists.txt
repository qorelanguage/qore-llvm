add_executable(qore-llvm
    driver.cpp
)

if(QORE_USE_LLVM)
    find_package(LLVM REQUIRED CONFIG)

    target_include_directories(qore-llvm PRIVATE ${LLVM_INCLUDE_DIRS})
    target_compile_definitions(qore-llvm PRIVATE QORE_USE_LLVM ${LLVM_DEFINITIONS})

    if(QORE_ENABLE_JIT)
        llvm_map_components_to_libnames(llvm_libs core bitwriter mcjit native)
        target_compile_definitions(qore-llvm PRIVATE QORE_ENABLE_JIT)
    else(QORE_ENABLE_JIT)
        llvm_map_components_to_libnames(llvm_libs core bitwriter)
    endif(QORE_ENABLE_JIT)

    target_compile_options(qore-llvm PRIVATE -fno-rtti)
endif(QORE_USE_LLVM)

target_link_libraries(qore-llvm
    common
    parser
    runtime
    scanner
    ${llvm_libs}
)
