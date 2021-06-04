#include <cstdio>
#include <igraph.h>

#include "options.hpp"
#include "graph_manager.hpp"


int main(int argc, char **argv)
{
    Option::OptionParser op_parser(argc, argv);
    Graph::GraphManager g_manager;
    igraph_t *g = g_manager.load_graph(op_parser.get_filename());
    igraph_t *gcc = g_manager.compute_gcc();

//    igraph_real_t diameter = 0;
//    igraph_diameter(gcc, &diameter, 0, 0, 0, IGRAPH_UNDIRECTED, 1);
//    std::cout << "diameter is: " << diameter << std::endl;

    return 0;
}
