#include "test_utils.h"
#include <lemon/list_graph.h>
#include "../MPC.h"
#include <stdlib.h>
#include <time.h>
#include "catch.hpp"

using namespace std;
using namespace lemon;



TEST_CASE("Feasible flow is found"){
    ListDigraph g;
    ListDigraph::NodeMap<int> labels(g);
    createRandomGraph(g, labels, 100, 0.9);
    addSourceAndSink(g);
    srand(time(NULL));
    
    ListDigraph::ArcMap<int> demands(g);
    for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
        demands[ai] = rand()%2;
    }
    ListDigraph::ArcMap<int> flow(g);
    find_feasible_flow(g, demands, flow);

    //check that the flow satisfies the demands:
    int f, d;
    for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
        f = flow[ai];
        d = demands[ai];
    	REQUIRE(f >= d);
    }

    //check for flow conservation; each node (Except s and t) receives and sends the same amount of flow
    for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    	int inflow = 0;
    	int outflow = 0;

    	//skip s and t
    	if(countInArcs(g, n) == 0 || countOutArcs(g, n) == 0){
    		continue;
    	}

    	for(ListDigraph::InArcIt ia(g, n); ia != INVALID; ++ia){
    		inflow += flow[ia];
    	}
    	for(ListDigraph::OutArcIt oa(g, n); oa != INVALID; ++oa){
    		outflow += flow[oa];
    	}

    	REQUIRE(inflow == outflow);
    }

}

