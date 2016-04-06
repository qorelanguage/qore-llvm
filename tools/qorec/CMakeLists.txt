add_executable(qorec
    qorec.cpp
)

target_link_libraries(qorec
    common
    comp
    rt
    in
    cg
)
