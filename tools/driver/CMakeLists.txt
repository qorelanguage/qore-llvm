find_package(BISON)
find_package(FLEX)
find_package(LLVM REQUIRED CONFIG)

bison_target(MyParser parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
flex_target(MyScanner lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(MyScanner MyParser)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp PROPERTIES COMPILE_FLAGS "-Wno-deprecated-register -Wno-unused-function -Wno-unneeded-internal-declaration")

add_executable(qore-llvm
    driver.cpp
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS}
)

target_include_directories(qore-llvm PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(qore-llvm PRIVATE ${LLVM_DEFINITIONS})
message(WARNING ${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs core mcjit native)
#llvm_map_components_to_libnames(llvm_libs core)

target_link_libraries(qore-llvm
    common
    runtime
    ${llvm_libs}
)

target_include_directories(qore-llvm PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
