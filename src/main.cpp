#include <cstdio>
#include <igraph.h>

#include "options.hpp"
#include "graph_manager.hpp"


int main(int argc, char **argv)
{
    Option::OptionParser op_parser(argc, argv);
    Graph::GraphManager g_manager;
    igraph_t *g = g_manager.load_graph(op_parser.get_filename());
    igraph_t *sub_g = g_manager.extract_subgraph(1, 20000);


    g_manager.flush();

    igraph_real_t diameter = 0;
    igraph_diameter(sub_g, &diameter, 0, 0, 0, IGRAPH_UNDIRECTED, 1);
    std::cout << "\n sub graph is composed by:\n"
        << igraph_vcount(sub_g) << " vertices\n"
        << igraph_ecount(sub_g) << " edges\n"
        <<"diameter of sub graph is: " << diameter << std::endl;

    //GCC du graph

    return 0;
}
