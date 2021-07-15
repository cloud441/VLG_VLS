#include <cstdio>
#include <igraph.h>

#include "options.hpp"
#include "graph_manager.hpp"

static void print_results(igraph_t *graph, igraph_t *span, std::string filename)
{
    std::cout << "\n\t_______________________________\n\n" << "Final results of the Spanner on graph: "
        << filename << "\n\n" << "original Greatest component (GCC) graph size:\n"
        << "\tnumber of vertices: " << igraph_vcount(graph) << '\n'
        << "\tnumber of edges: " << igraph_ecount(graph) << '\n'
        << "\nVery light spanner graph size:\n"
        << "\tnumber of vertices: " << igraph_vcount(span) << '\n'
        << "\tnumber of edges: " << igraph_ecount(span) << std::endl;
}


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
    igraph_t *span = g_manager.compute_spanner(Graph::GraphSource::GCC, op_parser.get_bfs_strategy(), op_parser.get_bfs_nb());

    // Print results:
    print_results(gcc, span, op_parser.get_filename());

    return 0;
}
