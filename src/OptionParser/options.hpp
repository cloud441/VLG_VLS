#pragma once

#include <string>
#include <iostream>
#include <filesystem>

namespace Option
{

    class OptionParser
    {
        public:

            OptionParser(int argc, char **argv);

            std::string get_filename();

        private:

            std::string filename;
            bool show_graph = false;
            bool debug = false;
    };


    inline std::string OptionParser::get_filename()
    {
        return this->filename;
    }

}; // namespace Option
