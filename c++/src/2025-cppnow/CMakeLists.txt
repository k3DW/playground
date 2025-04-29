add_library(2025_cppnow
    dummy.cpp
)
target_sources(2025_cppnow PUBLIC FILE_SET headers TYPE HEADERS FILES
    constant.hpp
    tuple.hpp
)

target_include_directories(2025_cppnow PUBLIC .)
set_target_properties(2025_cppnow PROPERTIES CXX_STANDARD 23)

add_subdirectory(test)
