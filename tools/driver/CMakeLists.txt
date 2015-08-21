find_package(BISON)
find_package(FLEX)

bison_target(MyParser parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
flex_target(MyScanner lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(MyScanner MyParser)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp PROPERTIES COMPILE_FLAGS "-Wno-deprecated-register -Wno-unused-function -Wno-unneeded-internal-declaration")

add_executable(qore-llvm
    driver.cpp
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS}
)

target_link_libraries(qore-llvm
    common
)

target_include_directories(qore-llvm PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
