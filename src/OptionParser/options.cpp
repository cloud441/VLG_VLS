#include "options.hpp"


namespace Option
{

    /**
     ** OptionParser class constructor:
     **     parse input and store them into class attributes.
     **/

    OptionParser::OptionParser(int argc, char **argv)
    {
        bool is_filename_specified = false;
        for (int i = 1; i < argc; i++)
        {
            if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help")
                print_help();

            if (std::string(argv[i]) == "-D")
                this->debug = true;

            else if (std::string(argv[i]) == "-S")
                this->show_graph = true;

            else if (std::string(argv[i]) == "-f")
            {
                i++;

                if (i == argc)
                    print_help();

                this->filename = std::string(argv[i]);
                is_filename_specified = true;

                if (!std::filesystem::exists(this->filename))
                {
                    std::cerr << "Error: graph path is wrong '" << this->filename << "'"<< std::endl;
                    exit(1);
                }
            }

            else if (std::string(argv[i]) == "--bfs-strategy")
            {
                i++;

                if (i == argc)
                    print_help();

                strategy_switch(std::string(argv[i]));
            }

            else if (argv[i][0] == '-')
            {
                std::cerr << "Error: unrecognize option: " << argv[i] << std::endl;
                exit(1);
            }
        }

        if (!is_filename_specified)
        {
            std::cerr << "Error: A filename must be specified with -f" << std::endl;
            exit(1);
        }
    }



    void OptionParser::strategy_switch(std::string strat)
    {
        if (strat == "random")
        {
            this->bfs_strategy = Spanner::BFS_STRATEGY::RANDOM;
        }
        else if (strat == "community")
        {
            this->bfs_strategy = Spanner::BFS_STRATEGY::COMMUNITY;
        }
        else
        {
            std::cerr << "Error: unrecognize bfs strategy '" << strat
                    << "'" << std::endl;
            exit(1);
        }
    }


    void print_help()
    {
        std::cout << "usage: ./vls <-f <graph_filename> > [-h/--help] [-S] [-D] [--bfs-strategy <strategy>]\n\n"
            << "Options description:\n" << "-f <graph_filename>:\t\tspecify the graph filename location.\n"
            << "-S:\t\t\t\tShow spanner graph in a human-readable way.\n"
            << "-D:\t\t\t\tPrint debug information during processing.\n"
            << "--bfs-strategy <strategy>:\tspecify the source selection BFS strategy for spanner computing.\n"
            << "\tPossible strategies:\n"
            << "\t\trandom:\t\tselect some source points randomly\n"
            << "\t\tcommunity:\tselect one source point by community in graph\n"
            << std::endl;

        exit(0);
    }

} // namespace Option
