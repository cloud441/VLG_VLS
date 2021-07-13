#pragma once

#include <igraph.h>
#include <vector>
#include <algorithm>
#include <iostream>


#define RNG_SEED 42
#define GAMMA_COMMUNITIES 0.0001


namespace Spanner
{

    enum BFS_STRATEGY
    {
        RANDOM,
        COMMUNITY
    };

    igraph_t *spanner_graph(igraph_t *g, BFS_STRATEGY strat);

} // namespace Spanner
