#include "test_utils.h"
#include <lemon/list_graph.h>
#include <lemon/cost_scaling.h>
#include "../MPC.h"
#include <stdlib.h>
#include <time.h>
#include "catch.hpp"

using namespace std;
using namespace lemon;


void flow_satisfies_demands(ListDigraph& g, ListDigraph::ArcMap<int>& flow, ListDigraph::ArcMap<int>& demand){
  int f, d;
  for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
    f = flow[ai];
    d = demand[ai];
    REQUIRE(f >= d);
  }
}

void check_flow_conservation(ListDigraph& g, ListDigraph::ArcMap<int>& flow){
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

TEST_CASE("simple test case succeeds"){
  ListDigraph g;
  ListDigraph::Node s = g.addNode();
  ListDigraph::Node t = g.addNode();
  ListDigraph::Node a = g.addNode();
  ListDigraph::Node b = g.addNode();
  ListDigraph::Node c = g.addNode();
  
  ListDigraph::Arc sa = g.addArc(s, a);
  ListDigraph::Arc sb = g.addArc(s, b);
  ListDigraph::Arc ac = g.addArc(a, c);
  ListDigraph::Arc bc = g.addArc(b, c);
  ListDigraph::Arc ct = g.addArc(c, t);
  ListDigraph::Arc bt = g.addArc(b, t);
  
  
  ListDigraph::ArcMap<int> demand(g);
  for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
    demand[ai] = 1;
  }
  ListDigraph::ArcMap<int> flow(g);
  
  find_minflow(g, demand, flow, s, t);
  
  REQUIRE(flow[ct] == 2);
  REQUIRE(flow[sa] == 1);
  REQUIRE(flow[sb] == 2);
  REQUIRE(flow[bc] == 1);
  REQUIRE(flow[ac] == 1);
  REQUIRE(flow[bt] == 1);
        
}
TEST_CASE("Another simple test case succeeds"){
  ListDigraph g;
  ListDigraph::Node s = g.addNode();
  ListDigraph::Node t = g.addNode();
  ListDigraph::Node a = g.addNode();
  ListDigraph::Node b = g.addNode();
  ListDigraph::Node c = g.addNode();
  ListDigraph::Node d = g.addNode();
  ListDigraph::Node e = g.addNode();
  
  ListDigraph::Arc sa = g.addArc(s, a);
  ListDigraph::Arc ac = g.addArc(a, c);
  ListDigraph::Arc cb = g.addArc(c, b);
  ListDigraph::Arc bt = g.addArc(b, t);
  ListDigraph::Arc sd = g.addArc(s, d);
  ListDigraph::Arc dc = g.addArc(d, c);
  ListDigraph::Arc ce = g.addArc(c, e);
  ListDigraph::Arc et = g.addArc(e, t);
  
  
  ListDigraph::ArcMap<int> demand(g);
  for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
    demand[ai] = 1;
  }
  ListDigraph::ArcMap<int> flow(g);
  
  find_minflow(g, demand, flow, s, t);
  
  REQUIRE(flow[bt]+flow[et] == 2);
        
}

TEST_CASE("A bit more complicated test case succeeds"){
  ListDigraph g;
  ListDigraph::Node s = g.addNode();
  ListDigraph::Node t = g.addNode();
  ListDigraph::Node a = g.addNode();
  ListDigraph::Node b = g.addNode();
  ListDigraph::Node c = g.addNode();
  ListDigraph::Node d = g.addNode();
  ListDigraph::Node e = g.addNode();
  ListDigraph::Node f = g.addNode();
  
  ListDigraph::Arc sa = g.addArc(s, a);
  ListDigraph::Arc sd = g.addArc(s, d);
  ListDigraph::Arc db = g.addArc(d, b);
  ListDigraph::Arc ab = g.addArc(a, b);
  ListDigraph::Arc de = g.addArc(d, e);
  ListDigraph::Arc bf = g.addArc(b, f);
  ListDigraph::Arc bc = g.addArc(b, c);
  ListDigraph::Arc ef = g.addArc(e, f); 
  ListDigraph::Arc ct = g.addArc(c, t);
  ListDigraph::Arc ft = g.addArc(f, t);
  
  
  ListDigraph::ArcMap<int> demand(g);
  for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
    demand[ai] = 1;
  }
  ListDigraph::ArcMap<int> flow(g);
  
  find_minflow(g, demand, flow, s, t);
  
  //check that total flow is minimum (3)
  REQUIRE(flow[ct]+flow[ft] == 3);
  
    
  
}

TEST_CASE("feasible minflow is generated for a random graph"){
  srand(time(NULL));
  ListDigraph g;
  createRandomGraph(g, 100, 0.9);
 
  ListDigraph::Node s, t;
  
  s = addSource(g);
  t = addSink(g);
      
  ListDigraph::ArcMap<int> demands(g);
  ListDigraph::ArcMap<int> flow(g);
  

  for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
    demands[ai] = rand()%2;
    flow[ai] = 0;
    g.target(ai);
  }
  
  find_minflow(g, demands, flow, s, t);
  
  flow_satisfies_demands(g, flow, demands);
    
  check_flow_conservation(g, flow);
  

}


TEST_CASE("Minflow value is correct for a random graph"){
  srand(time(NULL));
  ListDigraph g;
  createRandomGraph(g, 100, 0.9);
    
  ListDigraph::Node s, t;
  
  s = addSource(g);
  t = addSink(g);
 
    
  ListDigraph::ArcMap<int> demand(g);
  ListDigraph::ArcMap<int> cost(g);
  ListDigraph::ArcMap<int> flow(g);
  
  
  for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
    demand[ai] = 1;
    cost[ai] = 1;
  }
  
  CostScaling<ListDigraph> costScaling(g);
	costScaling.lowerMap(demand);
	costScaling.costMap(cost);

	//pushing 100 units of flow is just arbitrary number, it has to be equal or bigger than the minimum flow
	costScaling.stSupply(s, t, 1000);

	costScaling.run();
	
	find_minflow(g, demand, flow, s, t);
	
	int minflow_counter = 0;
	int maxflow_counter = 0;
	
	for(ListDigraph::InArcIt ia(g, s); ia != INVALID; ++ia){
	  
	  minflow_counter += costScaling.flow(ia);
	  maxflow_counter += flow[ia];
	}
	REQUIRE(minflow_counter == maxflow_counter);
  
}


TEST_CASE("Feasible flow is found for a random graph"){
    srand(time(NULL));
    ListDigraph g;
    createRandomGraph(g, 100, 0.9);
    
    ListDigraph::Node s, t;
  
    s = addSource(g);
    t = addSink(g);
    
    ListDigraph::ArcMap<int> demands(g);
    ListDigraph::ArcMap<int> flow(g);
    
    
    
    for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
        demands[ai] = rand()%2;
        flow[ai] = 0;
        g.target(ai);
    }
    
    find_feasible_flow(g, demands, flow);

    flow_satisfies_demands(g, flow, demands);
    
    check_flow_conservation(g, flow);

}

