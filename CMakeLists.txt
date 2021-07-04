cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

project( VLS )
include_directories( /usr/local/include/igraph/
                     src/
                     src/OptionParser/
                     src/GraphManager/
                     src/SpannerAlgo/
                     )


link_directories( /usr/local/lib/ )
add_executable( vls
    src/main.cpp
    src/OptionParser/options.cpp
    src/GraphManager/graph_manager.cpp
    src/SpannerAlgo/spanner_algo.cpp
    )


target_link_libraries( vls LINK_PUBLIC igraph )
