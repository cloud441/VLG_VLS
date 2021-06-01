#include "graph_manager.hpp"

namespace Graph
{

    graph::graph(std::string filename)
    {
        //Open a File pipe to graph filename:
        FILE *f;
        if ((f = fopen(filename.c_str(), "r")) == NULL)
        {
            std::cerr << "Error: Impossible to open the graph filename: "
                << filename
                << std::endl;
            exit(1);
        }

        // build the graph according to file syntax:
        char line[MAX_LINE_LENGTH];
        int i, u, v;
        //graph *g;

        //if( (g=(graph *)malloc(sizeof(graph))) == NULL )
        //    std::cerr << "graph_from_file: malloc() error 1");

        /* read n */
        if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
        {
            std::cerr << "graph_from_file: read error (fgets) 1" << std::endl;
            exit(1);
        }
        if( sscanf(line, "%d\n", &(this->n)) != 1 )
        {
            std::cerr << "graph_from_file: read error (sscanf) 2" << std::endl;
            exit(1);
        }

        /* read the degree sequence */
        if( (this->capacities=(int *)malloc(this->n*sizeof(int))) == NULL )
        {
            std::cerr << "graph_from_file: malloc() error 2" << std::endl;
            exit(1);
        }
        if( (this->degrees=(int *)calloc(this->n,sizeof(int))) == NULL )
        {
            std::cerr << "graph_from_file: calloc() error" << std::endl;
            exit(1);
        }
        for(i=0;i<this->n;i++){
            if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
            {
                std::cerr << "graph_from_file; read error (fgets) 2" << std::endl;
                exit(1);
            }
            if( sscanf(line, "%d %d\n", &v, &(this->capacities[i])) != 2 )
            {
                std::cerr << "graph_from_file; read error (sscanf) 2" << std::endl;
                exit(1);
            }
            if( v != i ){
                fprintf(stderr,"Line just read : %s\n i = %d; v = %d\n",line,i,v);
                std::cerr << "graph_from_file: error while reading degrees" << std::endl;
                exit(1);
            }
        }
    }
}
