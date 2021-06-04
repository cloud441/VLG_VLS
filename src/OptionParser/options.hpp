#pragma once

#include <string>
#include <iostream>
#include <filesystem>


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

            std::string filename; // graph filename to load.
            bool show_graph = false; // option to print graph during processing.
            bool debug = false; // option to print debug information during processing.
    };


    /**
     ** Getters implementation:
     **/

    inline std::string OptionParser::get_filename()
    {
        return this->filename;
    }

}; // namespace Option
