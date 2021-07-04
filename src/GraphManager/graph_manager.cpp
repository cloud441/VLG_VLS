#include "graph_manager.hpp"

namespace Graph
{

    /**
     ** GraphManager class constructor:
     **     allocate memory for pointer attributes.
     **/

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
        this->span = NULL;
        this->sub_graph = NULL;
    }


    /**
     ** GraphManager class destructor:
     **     free memory for pointer attributes.
     **/

    GraphManager::~GraphManager()
    {
        free(this->graph);
        free(this->edges);

        if (this->gcc)
            free(this->gcc);
    }


    /**
     ** load_graph():
     **     params: filename -> path of graph file.
     **
     **     Read graph from filename file and parse the vertices number
     **     and edge list.
     **/

    igraph_t *GraphManager::load_graph(std::string filename)
    {
        // Open a File pipe to graph filename:
        FILE *f;
        if ((f = fopen(filename.c_str(), "r")) == NULL)
        {
            std::cerr << "Error: Impossible to open the graph filename: "
                << filename
                << std::endl;
            exit(1);
        }

        // Build the graph according to file syntax:
        char line[MAX_LINE_LENGTH];
        int *degrees = NULL;
        int i, u, v;

        // Read number of vertices:
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

        // Read the degree sequence:
        if( (degrees=(int *)malloc(this->vertices_nb*sizeof(int))) == NULL )
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
            if( sscanf(line, "%d %d\n", &v, &(degrees[i])) != 2 )
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

        // Compute the number of edges:
        this->edges_nb=0;
        for(i=0;i<this->vertices_nb;i++)
            this->edges_nb += degrees[i];
        this->edges_nb /= 2;


        // Read edge list:
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

        // Create the igraph structure:
        igraph_create(this->graph, this->edges, this->vertices_nb, IGRAPH_UNDIRECTED);

        // Check the valid read of the graph file:
        if( fgets(line,MAX_LINE_LENGTH,f) != NULL )
        {
            std::cerr << "graph_from_file; too many lines" << std::endl;
            exit(1);
        }

        return this->graph;
    }


    /**
     ** extract_subgraph():
     **     params: first_vertex -> begin vertex ID of the sequence.
     **             last_vertex  -> end vertex ID of the sequence.
     **
     **     Extract a graph part and build another graph from the selected sequence.
     **     The sequence is a continuous part of vertex between first_vertex ID and
     **     last_vertex ID.
     **/

    igraph_t *GraphManager::extract_subgraph(int first_vertex, int last_vertex)
    {
        igraph_integer_t from = first_vertex;
        igraph_integer_t to = last_vertex;
        igraph_vs_t vs;

        igraph_vs_seq(&vs, from, to);

        igraph_t *sub_g = (igraph_t *)malloc(sizeof(igraph_t));
        if (!sub_g)
        {
            std::cerr << "Error: malloc failed to allocate sub graph space" << std::endl;
            exit(1);
        }

        igraph_induced_subgraph(this->graph, sub_g, vs, IGRAPH_SUBGRAPH_COPY_AND_DELETE);
        this->sub_graph = sub_g;

        return sub_g;
    }


    /**
     ** compute_gcc():
     **     Compute the greatest connected component of the graph.
     **     the objective is to avoid isolated vertices and
     **     obtain a graph with defined excentricity and diameter.
     **/

    igraph_t *GraphManager::compute_gcc()
    {
        if (this->gcc)
            return this->gcc;

        igraph_vector_t components;
        igraph_vector_t components_size;
        igraph_vector_init(&components, 1);
        igraph_vector_init(&components_size, 1);
        igraph_integer_t nb_components;

        igraph_clusters(this->graph, &components, &components_size, &nb_components, IGRAPH_STRONG);

        // get id of max component.
        int gcc_id = igraph_vector_which_max(&components_size);

        // get a point of the component
        int gcc_vertex_id = 0;
        while (VECTOR(components)[gcc_vertex_id] != gcc_id)
            gcc_vertex_id++;

        // compute GCC graph
        igraph_vector_t gcc_vertices;
        igraph_vector_init(&gcc_vertices, 1);
        igraph_vs_t vs;

        igraph_subcomponent(this->graph, &gcc_vertices, gcc_vertex_id, IGRAPH_ALL);
        igraph_vs_vector(&vs, &gcc_vertices);

        this->gcc = (igraph_t *)malloc(sizeof(igraph_t));
        igraph_induced_subgraph(this->graph, this->gcc, vs, IGRAPH_SUBGRAPH_COPY_AND_DELETE);

        return this->gcc;
    }


    /**
     ** compute_spanner():
     **     Compute the graph spanner according to algorithms in spanner_algo.cpp.
     **     A graph spanner in a light version of the graph containing same nulber of edges
     **     but less vertices. The objective is to lighten a graph regarding to the same
     **     structure.
     **/

    igraph_t *GraphManager::compute_spanner(GraphSource source = GraphSource::ORIGIN)
    {
        // Compute span from specific graph version (tests):
        switch(source)
        {
        case GraphSource::ORIGIN:
            this->span = Spanner::spanner_graph(this->graph);
            break;

        case GraphSource::GCC:
            this->span = Spanner::spanner_graph(this->gcc);
            break;

        case GraphSource::SUBGRAPH:
            this->span = Spanner::spanner_graph(this->sub_graph);
            break;

        default:
            break;
        }

        return this->span;
    }


    /**
     ** flush():
     **     print the graph basic informations.
     **/

    void GraphManager::flush()
    {
        std::cout << "graph is composed by:\n\n"
            << this->get_vertices_nb() << " vertices\n"
            << this->get_edges_nb() << " edges\n";
    }


    /**
     ** diameter():
     **     Get the graph diameter according to igraph algorithm.
     **/

    igraph_real_t diameter(igraph_t *g)
    {
        igraph_real_t d = 0;
        igraph_diameter(g, &d, 0, 0, 0, IGRAPH_UNDIRECTED, 1);

        return d;
    }


    /**
     ** diameter_path():
     **     Get the graph diameter path according to igraph algorithm.
     **/

    igraph_vector_t diameter_path(igraph_t *g)
    {
        igraph_vector_t path;
        igraph_vector_init(&path, 10);
        igraph_diameter(g, 0, 0, 0, &path, IGRAPH_UNDIRECTED, 1);

        return path;
    }
} // namespace Graph
