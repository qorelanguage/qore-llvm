cmake_minimum_required(VERSION 3.2)
project(QoreLLVM VERSION 1.0.0 LANGUAGES C CXX)

option(QORE_COVERAGE "Compile with coverage info" ON)
option(QORE_USE_LLVM "Generate LLVM IR code" ON)
option(QORE_ENABLE_JIT "Enable JIT (ignored when QORE_USE_LLVM is OFF)" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -fvisibility=hidden")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)

if(QORE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif(QORE_COVERAGE)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)

set(CPP_CHECK_OPTIONS --enable=all -I include -I /usr/local/include --config-exclude=/usr -i parser.cpp -i lexer.cpp --inline-suppr --std=c++11)
set(CPP_CHECK_FILES lib tools)
add_custom_target(cppcheck
    cppcheck ${CPP_CHECK_OPTIONS} --xml --xml-version=2 ${CPP_CHECK_FILES} 2> ${CMAKE_BINARY_DIR}/cppcheck.xml
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
add_custom_target(cppcheck-eclipse 
    cppcheck ${CPP_CHECK_OPTIONS} --template='{file}\:{line}\: {severity}\: {id} - {message}' ${CPP_CHECK_FILES} 2>&1 | sed 's/style/info/' | sed 's/information/info/'
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(test)
