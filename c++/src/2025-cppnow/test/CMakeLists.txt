add_executable(2025_cppnow_test test_main.cpp
    constant_ut.cpp
    overloaded_ut.cpp
    tuple_ut.cpp
)

target_link_libraries(2025_cppnow_test 2025_cppnow)
set_target_properties(2025_cppnow_test PROPERTIES CXX_STANDARD 23)
