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

        /* compute the number of links */
        this->m=0;
        for(i=0;i<this->n;i++)
            this->m += this->capacities[i];
        this->m /= 2;

        /* create contiguous space for links */
        if (this->n==0){
            this->links = NULL; this->degrees = NULL; this->capacities = NULL;
        }
        else {
            if( (this->links=(int **)malloc(this->n*sizeof(int*))) == NULL )
            {
                std::cerr << "graph_from_file: malloc() error 3" << std::endl;
                exit(1);
            }
            if( (this->links[0]=(int *)malloc(2*this->m*sizeof(int))) == NULL )
            {
                std::cerr << "graph_from_file: malloc() error 4" << std::endl;
                exit(1);
            }
            for(i=1;i<this->n;i++)
                this->links[i] = this->links[i-1] + this->capacities[i-1];
        }

        /* read the links */
        for(i=0;i<this->m;i++) {
            if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
            {
                std::cerr << "graph_from_file; read error (fgets) 3" << std::endl;
                exit(1);
            }
            if( sscanf(line, "%d %d\n", &u, &v) != 2 ){
                fprintf(stderr,"Attempt to scan link #%d failed. Line read:%s\n", i, line);
                std::cerr << "graph_from_file; read error (sscanf) 3" << std::endl;
                exit(1);
            }
            if ( (u>=this->n) || (v>=this->n) || (u<0) || (v<0) ) {
                fprintf(stderr,"Line just read: %s",line);
                std::cerr << "graph_from_file: bad node number" << std::endl;
                exit(1);
            }
            if ( (this->degrees[u]>=this->capacities[u]) ||
                    (this->degrees[v]>=this->capacities[v]) ){
                fprintf(stderr, "reading link %s\n", line);
                std::cerr << "graph_from_file: too many links for a node" << std::endl;
                exit(1);
            }
            this->links[u][this->degrees[u]] = v;
            this->degrees[u]++;
            this->links[v][this->degrees[v]] = u;
            this->degrees[v]++;
        }
        for(i=0;i<this->n;i++)
            if (this->degrees[i]!=this->capacities[i])
            {
                std::cerr << "graph_from_file: capacities <> degrees" << std::endl;
                exit(1);
            }
        if( fgets(line,MAX_LINE_LENGTH,f) != NULL )
        {
            std::cerr << "graph_from_file; too many lines" << std::endl;
            exit(1);
        }
    }


    void graph::flush()
    {
        std::cout << "graph is composed by:\n\n"
            << get_n() << " nodes\n"
            << get_m() << " edges\n"
            << "and first node is composed by:\n\t"
            << (get_degrees())[0] << " is the degree\n\t"
            << (get_capacities())[0] << " is the capacity\n";
    }
}
