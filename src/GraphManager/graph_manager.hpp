#pragma once

#include <string>
#include <iostream>
#include <cstdio>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1000


namespace Graph
{

    class graph
    {
        public:
            graph(std::string filename);


            void set_n(int n);
            void set_m(int m);
            void set_links(int **links);
            void set_degrees(int *degrees);
            void set_capacities(int *capacities);


            int get_n();
            int get_m();
            int **get_links();
            int *get_degrees();
            int *get_capacities();

        private:

            int n;
            int m;
            int **links;
            int *degrees;
            int *capacities;
    };


    /**
     ** Setters of graph class:
     **/

    inline void graph::set_n(int n)
    {
        this->n = n;
    }

    inline void graph::set_m(int m)
    {
        this->m = m;
    }

    inline void graph::set_links(int **links)
    {
        this->links = links;
    }

    inline void graph::set_degrees(int *degrees)
    {
        this->degrees = degrees;
    }

    inline void graph::set_capacities(int *capacities)
    {
        this->capacities = capacities;
    }


    /**
     ** Getters of graph class:
     **/

    inline int graph::get_n()
    {
        return this->n;
    }

    inline int graph::get_m()
    {
        return this->m;
    }

    inline int **graph::get_links()
    {
        return this->links;
    }

    inline int *graph::get_degrees()
    {
        return this->degrees;
    }

    inline int *graph::get_capacities()
    {
        return this->capacities;
    }
}; //namespace Graph


/**
 ** Utilities functions:
 **/
