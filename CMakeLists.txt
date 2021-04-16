cmake_minimum_required(VERSION 3.0.0)
project(librados-demo VERSION 0.1.0)

add_executable(demo librados-demo.cc)

include_directories(/usr/include)
target_link_libraries(demo rados)
