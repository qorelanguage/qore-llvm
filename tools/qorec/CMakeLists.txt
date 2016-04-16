add_executable(qorec
    qorec.cpp
    Interactive.cpp
)

target_link_libraries(qorec
    common
    comp
    rt
    in
    cg
)
