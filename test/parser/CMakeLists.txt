add_executable(parser-test
    test1.cpp
)

target_link_libraries(parser-test PRIVATE googletest parser runtime)

add_test(parser ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/parser-test)

add_dependencies(check parser-test)
