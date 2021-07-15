#pragma once

#include <string>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <igraph.h>

#include "spanner_algo.hpp"


// Macro used in load_graph() for file parsing:
#define MAX_LINE_LENGTH 1000


namespace Graph
{

    enum GraphSource
    {
        ORIGIN,
        GCC,
        SUBGRAPH
    };

    /**
     ** GraphManager class:
     **     Wrapper on igraph structure with additional tools like GCC computation etc ...
     **/

    class GraphManager
    {
        public:

            GraphManager();
            ~GraphManager();

            igraph_t *load_graph(std::string filename);
            igraph_t *extract_subgraph(int first_vertice, int last_vertices);
            igraph_t *compute_gcc();
            igraph_t *compute_spanner(GraphSource source, Spanner::BFS_STRATEGY strat, int bfs_nb);

            void flush();

            // Getters:
            int get_vertices_nb();
            int get_edges_nb();


        private:

            igraph_t *graph; // igraph structure of very large graph.
            igraph_t *gcc; // Greatest Connected Component of the graph attribute.
            igraph_t *sub_graph; // sub part of the graph attribute.
            igraph_t *span; // Spanner version of the graph attribuutes.
            igraph_vector_t *edges; // all edges of the graph attributes.

            int vertices_nb; // number of vertices in graph attributes.
            int edges_nb; // number of edges in graph attributes.

    };

    /**
     ** Getters implementation:
     **/

    inline int GraphManager::get_vertices_nb()
    {
        return this->vertices_nb;
    }

    inline int GraphManager::get_edges_nb()
    {
        return this->edges_nb;
    }

    /**
     ** Other graph useful functions:
     **/

    igraph_real_t diameter(igraph_t *g);
    igraph_vector_t diameter_path(igraph_t *g);

}; //namespace Graph
