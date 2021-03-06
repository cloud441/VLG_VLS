#pragma once

#include <string>
#include <iostream>
#include <filesystem>

#include "spanner_algo.hpp"


namespace Option
{

    /**
     ** OptionParser Class:
     **     Parse option parameters and store them.
     **/
    class OptionParser
    {
        public:

            OptionParser(int argc, char **argv);

            // Getters:
            std::string get_filename();
            Spanner::BFS_STRATEGY get_bfs_strategy();
            int get_bfs_nb();

        private:

            // Attributes:
            std::string filename; // graph filename to load.
            bool show_graph = false; // option to print graph during processing.
            bool debug = false; // option to print debug information during processing.
            Spanner::BFS_STRATEGY bfs_strategy = Spanner::BFS_STRATEGY::RANDOM;
            int bfs_nb = 50;

            // Methods:
            void strategy_switch(std::string strat);

    };


    /**
     ** Getters implementation:
     **/

    inline std::string OptionParser::get_filename()
    {
        return this->filename;
    }


    inline Spanner::BFS_STRATEGY OptionParser::get_bfs_strategy()
    {
        return this->bfs_strategy;
    }


    inline int OptionParser::get_bfs_nb()
    {
        return this->bfs_nb;
    }


    /**
     ** Useful functions:
     **/

    void print_help();

}; // namespace Option
