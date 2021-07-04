#include <cstdio>
#include <igraph.h>

#include "options.hpp"
#include "graph_manager.hpp"


int main(int argc, char **argv)
{
    // Read option parameters:
    Option::OptionParser op_parser(argc, argv);

    // Instance graph manager and load graph from file:
    Graph::GraphManager g_manager;
    igraph_t *g = g_manager.load_graph(op_parser.get_filename());

    // Compute Greatest connected component:
    //igraph_t *gcc = g_manager.compute_gcc();

    // Extract light sub graph for testing:
    igraph_t *sub_g = g_manager.extract_subgraph(1, 10000);

    // Create spanner of the graph:
    igraph_t *span = g_manager.compute_spanner(Graph::GraphSource::SUBGRAPH);
    std::cout << igraph_vcount(span) << std::endl;

    return 0;
}
