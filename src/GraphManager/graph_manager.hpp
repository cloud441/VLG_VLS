#pragma once

#include <string>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <igraph.h>

// Macro used in load_graph() for file parsing:
#define MAX_LINE_LENGTH 1000


namespace Graph
{

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

            void flush();

            // Getters:
            int get_vertices_nb();
            int get_edges_nb();


        private:

            igraph_t *graph; // igraph structure of very large graph.
            igraph_t *gcc; // Greatest Connected Component of the graph attribute.
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

}; //namespace Graph
