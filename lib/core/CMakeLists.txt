add_library(core STATIC
    BinaryOperator.cpp
    Conversion.cpp
    Data.cpp
    FunctionGroup.cpp
    SourceInfo.cpp
    String.cpp
    impl/BinaryOperators.cpp
    impl/Conversions.cpp
    util/Logging.cpp
)

target_link_libraries(core ${FFI_LIBRARY_PATH})
target_include_directories(core PRIVATE ${FFI_INCLUDE_PATH})
