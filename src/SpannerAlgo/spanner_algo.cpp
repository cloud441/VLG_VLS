#include "spanner_algo.hpp"


namespace Spanner
{

    static std::vector<int> select_points_randomly(igraph_t *g)
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


    static std::vector<int> select_bfs_points(igraph_t *g, BFS_STRATEGY strat)
    {
        std::vector<int> select_pts;

        switch(strat)
        {
        case BFS_STRATEGY::RANDOM:
            select_pts = select_points_randomly(g);
            break;
        case BFS_STRATEGY::COMMUNITY:
            std::cerr << "Error: Community not implemented yet." << std::endl;
            exit(1);
            break;
        }

        return select_pts;
    }


    /**
     ** initialize_spanner():
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
     ** build_bfs_edges():
     **     params:  vparents -> list of parent vertice index according to index in list.
     **
     **     Build the list of BFS edges following this structure:
     **         (parent_01, node_01, parent_02, node_02, ...)
     **/

    static igraph_vector_t build_bfs_edges(igraph_vector_t vparents, igraph_integer_t root_id)
    {
        int vparents_size = igraph_vector_size(&vparents);

        igraph_vector_t bfs_edges;
        igraph_vector_init(&bfs_edges, 2 * vparents_size);

        for (int i = 0; i < vparents_size; i++)
        {
            // skip the root check because root doesn't have a parent.
            if (i == root_id)
                continue;

            VECTOR(bfs_edges)[2 * i] = VECTOR(vparents)[i]; // parent
            VECTOR(bfs_edges)[2 * i + 1] = i; // child

            // manage root case by swapping parent/node order.
            // So, we have two (root, root_child) into bfs_edges.
            if (VECTOR(vparents)[i] == root_id)
            {
                VECTOR(bfs_edges)[2 * root_id] = i; // child
                VECTOR(bfs_edges)[2 * root_id + 1] = root_id; // root
            }
        }

        return bfs_edges;
    }


    /**
     ** merge_bfs():
     **     params:  g -> spanner graph we want to complete with new BFS.
     **              bfs_vertices -> vertices of the new BFS to merge.
     **              bfs_layers -> vertices layer numbers according to bfs_vertices order.
     **              bfs_vparents -> parent of each edges into new BFS search.
     **
     **/

    static void merge_bfs(igraph_t *g, igraph_vector_t bfs_vertices, igraph_vector_t bfs_layers, igraph_vector_t bfs_vparents)
    {
        // First merge case, just take all edges from first BFS
        if (!igraph_ecount(g))
        {
            igraph_vector_t bfs_edges = build_bfs_edges(bfs_vparents, VECTOR(bfs_vertices)[0]);
            igraph_add_edges(g, &bfs_edges, 0);
            return;
        }

        // FIXME: finish else case, with selecting best vertices.
    }



    /**
     ** spanner_graph():
     **     params:  g -> model based graph for span building.
     **
     **     The current algorithm select some points of the graph to perform BFS (Breadth-first search)
     **     and merge these output graphs. These operations result on a light sparse version of the graph,
     **     this is the graph spanner.
     **/

    igraph_t *spanner_graph(igraph_t *g, BFS_STRATEGY strat)
    {
        // random selection of source points
        std::vector<int> sources_pt = select_bfs_points(g, strat);

        // Initialize span with all edges but no vertices
        igraph_t *span = initialize_spanner(g);

        // Initialize BFS storage vectors
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
            merge_bfs(span, bfs_vertices, bfs_layers, bfs_vparents);
            //FIXME: finish algo with stop condition
        }

        return span;
    }

} // namespace Spanner
