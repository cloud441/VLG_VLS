#pragma once

#include <string>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <igraph.h>

#define MAX_LINE_LENGTH 1000


namespace Graph
{

    class GraphManager
    {
        public:

            GraphManager();
            ~GraphManager();

            igraph_t *load_graph(std::string filename);
            igraph_t *extract_subgraph(int first_vertice, int last_vertices);

            void flush();

            int get_vertices_nb();
            int get_edges_nb();


        private:

            igraph_t *graph;
            igraph_vector_t *edges;

            int vertices_nb;
            int edges_nb;

    };

    /**
     ** Getters of graph class:
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
