#include "spanner_algo.hpp"


namespace Spanner
{

    static std::vector<int> select_bfs_points(igraph_t *g)
    {
        igraph_rng_t *rng_generator = igraph_rng_default();
        igraph_rng_init(rng_generator, &igraph_rngtype_glibc2);
        igraph_rng_seed(rng_generator, RNG_SEED);

        std::vector<int> select_pts = std::vector<int>();
        igraph_integer_t vertices_nb = igraph_vcount(g);

        for (int i = 0; i < 15; i++)
            select_pts.push_back(igraph_rng_get_integer(rng_generator, 0, vertices_nb));

        return select_pts;
    }


    /**
     ** initalize_spanner():
     **     params:  g -> model based graph for span building.
     **
     **     Initialize span igraph structure for span building.
     **     The begin graph is composed by all g graph vertices but without edges.
     **/

    static igraph_t *initialize_spanner(igraph_t *g)
    {
        igraph_t *span = (igraph_t *)malloc(sizeof(igraph_t));
        igraph_empty(span, igraph_vcount(g), IGRAPH_UNDIRECTED);

        return span;
    }



    /**
     ** spanner_graph():
     **     params:  g -> model based graph for span building.
     **
     **     The current algorithm select some points of the graph to perform BFS (Breadth-first search)
     **     and merge these output graphs. These operations result on a light sparse version of the graph,
     **     this is the graph spanner.
     **/

    igraph_t *spanner_graph(igraph_t *g)
    {
        // random selection of source points
        std::vector<int> sources_pt = select_bfs_points(g);

        // Initialize span (H) with all edges but no vertices
        igraph_t *span = initialize_spanner(g);

        // Initialize BFS storage vectors:
        igraph_vector_t bfs_vertices;
        igraph_vector_t bfs_layers;
        igraph_vector_t bfs_vparents;

        igraph_vector_init(&bfs_vertices, 0);
        igraph_vector_init(&bfs_layers, 0);
        igraph_vector_init(&bfs_vparents, 0);

        /* for each source points, compute BFS and merge it to span, compute difference of up/down bounding excentricity,
        compute mean value and variance and choose a stop condition. */
        for (int source_pt : sources_pt)
        {
            igraph_bfs_simple(g, source_pt, IGRAPH_ALL, &bfs_vertices, &bfs_layers, &bfs_vparents);
            /*
            FIXME: finish algo with merge and stop condition
            merge_bfs(span, bfs_vertices, bfs_layers, bfs_vparents);
            */
        }

        return span;
    }

} // namespace Spanner
