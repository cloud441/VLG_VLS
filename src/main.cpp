#include <cstdio>
#include <igraph.h>

#include "options.hpp"
#include "graph_manager.hpp"


int main(int argc, char **argv)
{
    Option::OptionParser op_parser(argc, argv);
    Graph::graph *g = new Graph::graph(op_parser.get_filename());

    g->flush();

    return 0;
}
