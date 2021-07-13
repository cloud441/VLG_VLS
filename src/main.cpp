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
    igraph_t *gcc = g_manager.compute_gcc();

    // Extract light sub graph for testing:
    //igraph_t *sub_g = g_manager.extract_subgraph(0, 10000);

    // Create spanner of the graph:
//    std::cout << "GCC vertices number: " << igraph_vcount(gcc) << '\n'
//        << "GCC edges number: " << igraph_ecount(gcc) << std::endl;

    igraph_t *span = g_manager.compute_spanner(Graph::GraphSource::GCC, op_parser.get_bfs_strategy());

//    std::cout << "\nSpan vertices number: " << igraph_vcount(span) << '\n'
//        << "Span edges number: " << igraph_ecount(span) << std::endl;

    return 0;
}
