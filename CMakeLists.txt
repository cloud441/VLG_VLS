cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 14)

project( VLS )
include_directories( /usr/local/include/igraph/ )
link_directories( /usr/local/lib/ )
add_executable( vls
    src/main.cpp)


target_link_libraries( vls LINK_PUBLIC igraph )
