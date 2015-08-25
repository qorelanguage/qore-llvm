#Setup GoogleTest and GoogleMock

find_package(Threads REQUIRED)
include(ExternalProject)
ExternalProject_Add(qgtest URL http://googletest.googlecode.com/files/gtest-1.7.0.zip PREFIX ${CMAKE_BINARY_DIR}/ext INSTALL_COMMAND "")
ExternalProject_Add(qgmock URL http://googlemock.googlecode.com/files/gmock-1.7.0.zip PREFIX ${CMAKE_BINARY_DIR}/ext INSTALL_COMMAND "")

#Forcing exact location in order to avoid using LLVM's (patched) copy of gtest
add_library(googletest-gtest STATIC IMPORTED)
set_target_properties(googletest-gtest PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/ext/src/qgtest-build/libgtest.a)

add_library(googletest-gmock STATIC IMPORTED)
set_target_properties(googletest-gmock PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/ext/src/qgmock-build/libgmock.a)

add_library(googletest INTERFACE)
target_link_libraries(googletest INTERFACE googletest-gtest googletest-gmock ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(googletest INTERFACE ${CMAKE_BINARY_DIR}/ext/src/qgtest/include ${CMAKE_BINARY_DIR}/ext/src/qgmock/include)

enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

add_subdirectory(parser)
