file(GLOB_RECURSE QORE_TEST_SOURCES "*.cpp")

add_executable(unittests ${QORE_TEST_SOURCES})

target_link_libraries(unittests
    common
    comp
    gtest
    gmock
)

add_test(unittests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unittests)
