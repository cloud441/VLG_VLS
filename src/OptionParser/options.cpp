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
            if (std::string(argv[i]) == "-D")
                this->debug = true;

            else if (std::string(argv[i]) == "-S")
                this->show_graph = true;

            else if (std::string(argv[i]) == "-f")
            {
                i++;
                this->filename = std::string(argv[i]);
                is_filename_specified = true;

                if (!std::filesystem::exists(this->filename))
                {
                    std::cerr << "Error: graph path is wrong: " << this->filename << std::endl;
                    exit(1);
                }
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

} // namespace Option
