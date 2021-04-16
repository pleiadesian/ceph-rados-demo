cmake_minimum_required(VERSION 3.0.0)
project(librados-demo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Set this path to your Ceph path
set(CEPH_HOME /home/wzl/ceph)

include_directories(/usr/include)
include_directories(${CEPH_HOME}/src/include)
link_directories(${CEPH_HOME}/build/lib)

add_executable(demo librados-demo.cc)

target_link_libraries(demo rados)
