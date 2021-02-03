project(ct_serialize)

set(CMAKE_CXX_STANDARD 17)

add_library(ct_ser_test test.cpp)
target_include_directories(ct_ser_test PRIVATE 
    ${CMAKE_SOURCE_DIR}
)