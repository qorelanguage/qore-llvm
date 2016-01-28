file(GLOB_RECURSE QORE_TEST_SOURCES "*.cpp")

add_executable(unittests ${QORE_TEST_SOURCES})

target_link_libraries(unittests
    common
    comp
    gtest
    gmock
)

add_test(unittests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unittests)

set(QORE_TEST_INPUT_DIR ${CMAKE_SOURCE_DIR}/qtif)
file(GLOB_RECURSE QORE_TEST_INPUT_FILE_LIST ${QORE_TEST_INPUT_DIR}/*.qtif)

set(QORE_TEST_INPUT_FILES "")
foreach(file ${QORE_TEST_INPUT_FILE_LIST})
    set(QORE_TEST_INPUT_FILES "${QORE_TEST_INPUT_FILES}QTIF(\"${file}\")\n")
endforeach()

configure_file(test_files.inc.in test_files.inc)

target_include_directories(unittests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
