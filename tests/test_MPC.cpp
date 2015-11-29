#include "catch.hpp"
#include "test_utils.h"
#include <lemon/list_graph.h>
#include "../MPC.h"
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace lemon;



TEST_CASE("Generating a feasible flow"){
    ListDigraph g;
    createRandomGraph(g, 100, 0.9);
    addSourceAndSink(g);
    srand(time(NULL));
    
    ListDigraph::ArcMap<int> demands(g);
    for(ListDigraph::ArcIt ai; ai != INVALID; ++ai){
        demands[ai] = rand()%2;
    }
    ListDigraph::ArcMap<int> flow(g);
    find_feasible_flow(g, demands, flow);

    //check that the flow satisfies the demands:
    for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
    	REQUIRE(flow[ai] >= demands[ai]);
    }

    //check for flow conservation; each node (Except s and t) receives and sends the same amount of flow
    for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    	int inflow = 0;
    	int outflow = 0;

    	//skip s and t
    	if(countInArcs(g, n) == 0 || countOutArcs(g, n) == 0){
    		continue;
    	}

    	for(ListDigraph::InArcIt ia(n); ia != INVALID; ++ia){
    		inflow += flow[ia];
    	}
    	for(ListDigraph::OutArcIt oa(n); oa != INVALID; ++oa){
    		outflow += flow[oa];
    	}

    	REQUIRE(inflow == outflow);
    }


}

