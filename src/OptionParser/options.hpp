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

        private:

            // Attributes:
            std::string filename; // graph filename to load.
            bool show_graph = false; // option to print graph during processing.
            bool debug = false; // option to print debug information during processing.
            Spanner::BFS_STRATEGY bfs_strategy = Spanner::BFS_STRATEGY::RANDOM;

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


    /**
     ** Useful functions:
     **/

    void print_help();

}; // namespace Option
