cmake_minimum_required(VERSION 3.2)
project(QoreLLVM VERSION 1.0.0 LANGUAGES C CXX)

include(cmake/define-options.txt)
include(cmake/gtest-include.txt)
include(cmake/compiler-config.txt)
include(cmake/coverage.txt)
include(cmake/doxygen.txt)
include(cmake/cppcheck.txt)
include(cmake/llvm.txt)
include(cmake/ffi.txt)

enable_testing()

add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(test)
