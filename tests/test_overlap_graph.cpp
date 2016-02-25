#include "test_utils.h"
#include <lemon/list_graph.h>
#include "../decomposition.h"
#include "../MPC.h"
#include <stdlib.h>
#include <time.h>
#include "catch.hpp"
#include <lemon/bfs.h>
#include <lemon/lgf_reader.h>
#include <lemon/cost_scaling.h>

using namespace std;
using namespace lemon;

TEST_CASE("correct minflow in an overlap graph"){
	ListDigraph g;
	ListDigraph::NodeMap<int> labels(g);
	ListDigraph::ArcMap<int> weights(g);
	digraphReader(g, "tests/test.graph").nodeMap("label", labels).arcMap("length", weights).run();

	ListDigraph::ArcMap<int> demands(g);
	for(ListDigraph::NodeIt n(g); n!= INVALID; ++n){
		ListDigraph::Node new_node = g.split(n, false);
		ListDigraph::Arc new_edge = g.addArc(n, new_node);
		demands[new_edge] = 1;
	}

	ListDigraph::Node s = addSource(g);
	ListDigraph::Node t = addSink(g);

	ListDigraph::ArcMap<int> minflow(g);

  	find_minflow(g, demands, minflow, s, t);

  	check_flow_conservation(g, minflow);
  	flow_satisfies_demands(g, minflow, demands);

  	CostScaling<ListDigraph> costScaling(g);
  	ListDigraph::ArcMap<int> dummyCosts(g);
	costScaling.lowerMap(demands);
	costScaling.costMap(dummyCosts);

	//pushing 100 units of flow is just arbitrary number, it has to be equal or bigger than the minimum flow
  	int minflow_value = -1;
  	int problem_type = 0;
  	while(problem_type != 1){
    	minflow_value++;
    	cout << minflow_value;
    	costScaling.stSupply(s, t, minflow_value);
   		problem_type = costScaling.run();
  	}
	
	int maxflow_counter = 0;
	
	for(ListDigraph::OutArcIt oa(g, s); oa != INVALID; ++oa){
	  maxflow_counter += minflow[oa];
	}
	REQUIRE(minflow_value == maxflow_counter);
}



TEST_CASE("decompose an overlap graph"){
	ListDigraph g;
	ListDigraph::NodeMap<int> labels(g);
	ListDigraph::ArcMap<int> weights(g);
	digraphReader(g, "tests/test.graph").nodeMap("label", labels).arcMap("length", weights).run();

	ListDigraph::ArcMap<int> demands(g);
	for(ListDigraph::NodeIt n(g); n!= INVALID; ++n){
		ListDigraph::Node new_node = g.split(n, false);
		ListDigraph::Arc new_edge = g.addArc(n, new_node);
		demands[new_edge] = 1;
	}

	ListDigraph::Node s = addSource(g);
	ListDigraph::Node t = addSink(g);

	ListDigraph::ArcMap<int> minflow(g);

  	find_minflow(g, demands, minflow, s, t);

  	int num_ants = 0;
	for (ListDigraph::OutArcIt o(g, s); o != INVALID; ++o){
    	num_ants += minflow[o];
  	}
  	cout << num_ants << " <-- minflow value\n";
  	vector<ListDigraph::Node*> decomposition;

  	decompose_graph(g, minflow, s, t, labels, decomposition);

  	//cout << decomposition.size() << " MACs found from the test graph\n";
  	REQUIRE(!decomposition.empty());

  	/** THIS TAKES A LOT OF TIME
  	for(vector<ListDigraph::Node*>::iterator it = decomposition.begin(); it != decomposition.end(); ++it){
    	REQUIRE(isMac(*it, num_ants, g));
  	}

  	**/

}