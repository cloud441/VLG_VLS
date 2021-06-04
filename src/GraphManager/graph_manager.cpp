#include "graph_manager.hpp"

namespace Graph
{

    GraphManager::GraphManager()
    {
        this->graph = (igraph_t *)malloc(sizeof(igraph_t));
        if (!(this->graph))
        {
            std::cerr << "Error: malloc can't allocate graph object" << std::endl;
            exit(1);
        }

        this->edges = (igraph_vector_t *)malloc(sizeof(igraph_vector_t));
        if (!(this->edges))
        {
            std::cerr << "Error: malloc can't allocate edges object" << std::endl;
            exit(1);
        }

        this->gcc = NULL;
    }


    GraphManager::~GraphManager()
    {
        free(this->graph);
        free(this->edges);
        if (this->gcc)
            free(this->gcc);
    }




    igraph_t *GraphManager::load_graph(std::string filename)
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
        int *capacities = NULL;
        int i, u, v;

        /* read n */
        if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
        {
            std::cerr << "graph_from_file: read error (fgets) 1" << std::endl;
            exit(1);
        }
        if( sscanf(line, "%d\n", &(this->vertices_nb)) != 1 )
        {
            std::cerr << "graph_from_file: read error (sscanf) 2" << std::endl;
            exit(1);
        }

        /* read the degree sequence */
        if( (capacities=(int *)malloc(this->vertices_nb*sizeof(int))) == NULL )
        {
            std::cerr << "graph_from_file: malloc() error 2" << std::endl;
            exit(1);
        }

        for(i=0;i<this->vertices_nb;i++){
            if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
            {
                std::cerr << "graph_from_file; read error (fgets) 2" << std::endl;
                exit(1);
            }
            if( sscanf(line, "%d %d\n", &v, &(capacities[i])) != 2 )
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
        this->edges_nb=0;
        for(i=0;i<this->vertices_nb;i++)
            this->edges_nb += capacities[i];
        this->edges_nb /= 2;


        /* read the links */

        igraph_vector_init(this->edges, this->edges_nb * 2);

        for(i=0;i<this->edges_nb;i++) {
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
            if ( (u>=this->vertices_nb) || (v>=this->vertices_nb) || (u<0) || (v<0) ) {
                fprintf(stderr,"Line just read: %s",line);
                std::cerr << "graph_from_file: bad node number" << std::endl;
                exit(1);
            }

            VECTOR(*(this->edges))[2 * i] = u;
            VECTOR(*(this->edges))[2 * i + 1] = v;
        }

        igraph_create(this->graph, this->edges, this->vertices_nb, false);

        if( fgets(line,MAX_LINE_LENGTH,f) != NULL )
        {
            std::cerr << "graph_from_file; too many lines" << std::endl;
            exit(1);
        }

        return this->graph;
    }


    igraph_t *GraphManager::extract_subgraph(int first_vertice, int last_vertices)
    {
        igraph_integer_t from = first_vertice;
        igraph_integer_t to = last_vertices;
        igraph_vs_t vs;

        igraph_vs_seq(&vs, from, to);

        igraph_t *sub_g = (igraph_t *)malloc(sizeof(igraph_t));
        if (!sub_g)
        {
            std::cerr << "Error: malloc failed to allocate sub graph space" << std::endl;
            exit(1);
        }

        igraph_induced_subgraph(this->graph, sub_g, vs, IGRAPH_SUBGRAPH_COPY_AND_DELETE);
        return sub_g;
    }


    igraph_t *GraphManager::compute_gcc()
    {
        if (this->gcc)
            return this->gcc;

        igraph_vector_t components;
        igraph_vector_t components_size;
        igraph_vector_t gcc_vertices;
        igraph_integer_t nb_components;
        igraph_vs_t vs;
        this->gcc = (igraph_t *)malloc(sizeof(igraph_t));

        igraph_vector_init(&components, 1);
        igraph_vector_init(&components_size, 1);
        igraph_clusters(this->graph, &components, &components_size, &nb_components, IGRAPH_STRONG);


        // get id of max component.
        int gcc_id = igraph_vector_which_max(&components_size);


        // get a point of the component
        int gcc_vertex_id = 0;
        while (VECTOR(components)[gcc_vertex_id] != gcc_id)
            gcc_vertex_id++;


        // compute GCC graph
        igraph_vector_init(&gcc_vertices, 1);
        igraph_subcomponent(this->graph, &gcc_vertices, gcc_vertex_id, IGRAPH_ALL);
        igraph_vs_vector(&vs, &gcc_vertices);
        igraph_induced_subgraph(this->graph, this->gcc, vs, IGRAPH_SUBGRAPH_COPY_AND_DELETE);

        return this->gcc;
    }



    void GraphManager::flush()
    {
        std::cout << "graph is composed by:\n\n"
            << this->get_vertices_nb() << " vertices\n"
            << this->get_edges_nb() << " edges\n";
    }
}
