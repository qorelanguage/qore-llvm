find_package(LLVM REQUIRED CONFIG)

add_executable(qore-llvm
    driver.cpp
)

target_include_directories(qore-llvm PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(qore-llvm PRIVATE ${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs core bitwriter mcjit native)

target_link_libraries(qore-llvm
    common
    parser
    runtime
    ${llvm_libs}
)
