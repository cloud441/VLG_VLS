#pragma once

#include <igraph.h>
#include <vector>
#include <iostream>


#define RNG_SEED 42


namespace Spanner
{

    enum BFS_STRATEGY
    {
        RANDOM,
        COMMUNITY
    };

    igraph_t *spanner_graph(igraph_t *g);

} // namespace Spanner
