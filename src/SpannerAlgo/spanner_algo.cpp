#include "spanner_algo.hpp"


namespace Spanner
{

    /**
     ** vector_mean():
     **     params:  vec -> vector to compute the mean value.
     **
     **     Compute mean value of int vector.
     **/

    static float vector_mean(std::vector<int> vec)
    {
        float mean = 0.0f;

        if (!vec.empty())
            mean = std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();

        return mean;
    }


    /**
     ** vector_var():
     **     params:  vec -> vector to compute the variance value.
     **
     **     Compute variance value of int vector.
     **/

    static float vector_var(std::vector<int> vec)
    {
        std::vector<float> vec_f = std::vector<float>(vec.begin(), vec.end());
        float mean = vector_mean(vec);
        float variance = 0.0f;

        std::for_each(vec_f.begin(), vec_f.end(), [mean](float& elt) { elt -= mean;});

        if (!vec_f.empty())
            variance = std::inner_product(vec_f.begin(), vec_f.end(), vec_f.begin(), 0.0) / vec.size();

        return variance;
    }


    /**
     ** select_points_randomly():
     **     params:  g -> model based graph for span building.
     **
     **     Select 15 random vertices from graph.
     **/

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


    /**
     ** select_points_from_communities():
     **     params:  g -> model based graph for span building.
     **
     **     Compute communities of the graph based on Leiden algorithm, then select one
     **     representative from each community. The result is all the selected vertices
     **     of the graph.
     **/

    static std::vector<int> select_points_from_communities(igraph_t *g)
    {
        // Compute communities
        igraph_integer_t communities_nb;
        igraph_vector_t membership;
        igraph_vector_init(&membership, igraph_vcount(g));

        igraph_community_leiden(g, NULL, NULL, GAMMA_COMMUNITIES, 0.01, 0, &membership, &communities_nb, NULL);

        // Add communities representative (first encountered) as source points
        std::vector<int> select_pts = std::vector<int>();
        std::vector<int> encountered_clusters = std::vector<int>();
        int membership_index = 0;

        while (select_pts.size() < communities_nb)
        {
            int cluster = VECTOR(membership)[membership_index];

            if (std::find(encountered_clusters.begin(), encountered_clusters.end(), cluster) == encountered_clusters.end())
            {
                encountered_clusters.push_back(cluster);
                select_pts.push_back(membership_index);
            }

            membership_index++;
        }

        return select_pts;
    }


    /**
     ** select_bfs_points():
     **     params:  g -> model based graph for span building.
     **              strat -> strategy of points selection.
     **
     **     Select vertices from the g graph according to a strat strategy defined
     **     in BFS_STRATEGY enum. Then call the according strategy selection.
     **/

    static std::vector<int> select_bfs_points(igraph_t *g, BFS_STRATEGY strat)
    {
        std::vector<int> select_pts;

        switch(strat)
        {
        case BFS_STRATEGY::RANDOM:
            select_pts = select_points_randomly(g);
            break;
        case BFS_STRATEGY::COMMUNITY:
            select_pts = select_points_from_communities(g);
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
     ** remove_similar_edges():
     **     params:  g -> original graph containing last added edges.
     **              bfs_edges -> new BFS edges vector to clean.
     **
     **     Remove all edges from bfs_edges that are found in the original g graph.
     **/

    static void remove_similar_edges(igraph_t *g, igraph_vector_t *bfs_edges)
    {
        igraph_vector_t cleaned_edges;
        int bfs_edges_size = igraph_vector_size(bfs_edges);
        igraph_vector_init(&cleaned_edges, bfs_edges_size);

        igraph_integer_t edge_eid;
        int cleaned_index = 0;

        for (int i = 0; i < bfs_edges_size; i += 2)
        {
            igraph_integer_t from = VECTOR(*bfs_edges)[i];
            igraph_integer_t to = VECTOR(*bfs_edges)[i + 1];

            igraph_get_eid(g, &edge_eid, from, to, false, false);

            // Edge between from and to doesn't exist
            if (edge_eid == -1)
            {
                VECTOR(cleaned_edges)[cleaned_index++] = from;
                VECTOR(cleaned_edges)[cleaned_index++] = to;
            }
        }

        *bfs_edges = cleaned_edges;
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
        igraph_vector_t bfs_edges;
        bfs_edges = build_bfs_edges(bfs_vparents, VECTOR(bfs_vertices)[0]);

        // First merge case, just take all edges from first BFS
        if (!igraph_ecount(g))
        {
            igraph_add_edges(g, &bfs_edges, 0);
            return;
        }

        // Merge BFS with current g graph
        //        remove_similar_edges(g, &bfs_edges);

        igraph_add_edges(g, &bfs_edges, 0);
    }


    /**
     ** first_existing_source():
     **     params:  g_vertices -> spanner graph we want to complete with new BFS.
     **              sources_pt -> list of sources vertices from BFS strategy.
     **              index_in_sources_pt -> index of last selected point vertice.
     **
     **     Find the first source vertice index of sources_pt that is unused and is not removed of
     **     g_vertices during last function calls. Unused means that we never return this index by
     **     this function during last calls, and not removed means that was not erased by computing
     **     bounding eccentricities.
     **     In fact, this function returns the required vertice index and increments the index_in_sources_pt.
     **
     **/

    static int first_existing_source(std::vector<int> g_vertices, std::vector<int> sources_pt, int *index_in_sources_pt)
    {
        // we not use the last returned value
        (*index_in_sources_pt)++;

        std::vector<int>::iterator res_it;

        while ((res_it = std::find(g_vertices.begin(), g_vertices.end(), sources_pt[*index_in_sources_pt])) == g_vertices.end())
            (*index_in_sources_pt)++;

        // return value store into res_it iterator (ie. the first available source vertice index)
        return *res_it;
    }


    /**
     ** dijkstra_dist():
     **     params:  g -> spanner graph we want to complete with new BFS.
     **              from -> source vertice id of the required path.
     **              to -> destination vertice id of the required path.
     **
     **     Compute the dijkstra distance between from vertice to to vertice.
     **/

    static int dijkstra_dist(igraph_t *g, int from, int to)
    {
        igraph_matrix_t dist_mat;
        igraph_vs_t from_vs;
        igraph_vs_t to_vs;

        igraph_matrix_init(&dist_mat, 1, 1);
        igraph_vs_1(&from_vs, from);
        igraph_vs_1(&to_vs, to);

        igraph_shortest_paths(g, &dist_mat, from_vs, to_vs, IGRAPH_ALL);

        return igraph_matrix_e(&dist_mat, 0, 0);
    }



    /**
     ** bounding_eccentricities():
     **     params:  g -> spanner graph we want to complete with new BFS.
     **              sources_pt -> list of vertices index according to BFS sources
     **                             points selection
     **
     **     Return the list of all eccentricities associated to each vertices in g graph.
     **     The algorithm is based on Takes & Koster implementation.
     **/

    static std::vector<int> bounding_eccentricities(igraph_t *g, std::vector<int> sources_pt)
    {
        int vertices_nb = igraph_vcount(g);
        std::vector<int> g_vertices = std::vector<int>(vertices_nb);
        std::iota(g_vertices.begin(), g_vertices.end(), 0); // fill g_vertices with int from 0 to vertices_nb

        // define eccentricities vectors
        std::vector<int> eps = std::vector<int>(vertices_nb); // eccentricities list
        std::vector<int> eps_l = std::vector<int>(vertices_nb); // lower bound eccentricities list
        std::vector<int> eps_u = std::vector<int>(vertices_nb); // upper bound eccentricities list

        // set to default value
        std::fill(eps.begin(), eps.end(), 0);
        std::fill(eps_l.begin(), eps_l.end(), std::numeric_limits<int>::min());
        std::fill(eps_u.begin(), eps_u.end(), std::numeric_limits<int>::max());

        // define index of source vertice selected to compute eccentricity
        int source_pt_index = 0;
        int cur_vertice;

        while (!g_vertices.empty())
        {
            // select a based vertice to compute eccentricity, according to BFS strategy (communities, etc ...)
            cur_vertice = first_existing_source(g_vertices, sources_pt, &source_pt_index);
            igraph_vs_t ecc_help_vs;
            igraph_vs_1(&ecc_help_vs, cur_vertice);


            // compute eccentricity of cur_vector
            igraph_vector_t ecc_help_results;
            igraph_vector_init(&ecc_help_results, 0);
            igraph_eccentricity(g, &ecc_help_results, ecc_help_vs, IGRAPH_ALL);
            eps[cur_vertice] = VECTOR(ecc_help_results)[0];

            for (int v : g_vertices)
            {
                eps_l[cur_vertice] = std::max(eps_l[cur_vertice], std::max(eps[v] - dijkstra_dist(g, v, cur_vertice), dijkstra_dist(g, v, cur_vertice)));
                eps_u[cur_vertice] = std::min(eps_u[cur_vertice], eps[v] - dijkstra_dist(g, v, cur_vertice));

                if (eps_l[cur_vertice] == eps_u[cur_vertice])
                {
                    eps[cur_vertice] = eps_l[cur_vertice];
                    // erase cur_vertice of the vertices vectors
                    g_vertices.erase(std::find(g_vertices.begin(), g_vertices.end(), cur_vertice));
                }
            }
        }

        return eps;
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

        std::vector<int>  ecc_vector;

        /* for each source points, compute BFS and merge it to span, compute difference of up/down bounding excentricity,
           compute mean value and variance and choose a stop condition. */
        for (int i = 0 ; i < sources_pt.size(); i++)
        {
            std::cout << "Spanner building: BFS number " << i << " is merging ..." << '\n';
            igraph_bfs_simple(g, sources_pt[i], IGRAPH_ALL, &bfs_vertices, &bfs_layers, &bfs_vparents);

            if (0.8 * igraph_ecount(g) < (igraph_ecount(span) + (igraph_vector_size(&bfs_vparents) * 2)))
            {
                std::cout << "Stopping condition is reached." << std::endl;
                break;
            }

            merge_bfs(span, bfs_vertices, bfs_layers, bfs_vparents);
            std::cout << "Spanner is composed by: " << igraph_ecount(span) << " edges.\n" 
                << "merge is done." << '\n';

            std::cout << "Computing bounding eccentricities ..." << '\n';
            ecc_vector = bounding_eccentricities(g, sources_pt);
            std::cout << "mean of eccentricities of the spanner is: "  << vector_mean(ecc_vector)<< '\n'
                << "variance of eccentricities of the spanner is: " << vector_var(ecc_vector) << '\n';
        }


        std::cout << "Final very light spanner estimated diameter (by eccentricities maximum) is: "
            << *(std::max_element(ecc_vector.begin(), ecc_vector.end())) << std::endl;
        return span;
    }

} // namespace Spanner
