add_executable(qorec
    qorec.cpp
)

target_link_libraries(qorec
    common
    comp
)

setup_llvm(qorec)
