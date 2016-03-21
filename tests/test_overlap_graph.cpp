#include <lemon/list_graph.h>
#include "../decomposition.h"
#include "../MPC.h"
#include <stdlib.h>
#include <time.h>
#include <lemon/bfs.h>
#include <lemon/lgf_reader.h>
#include <lemon/cost_scaling.h>
#include <string.h>
#include <lemon/adaptors.h>
#include <lemon/connectivity.h>

using namespace std;
using namespace lemon;

ListDigraph::Node addSource(ListDigraph& g){

  ListDigraph::Node s = g.addNode();
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    
        if(countInArcs(g, n) == 0 && n != s){
            g.addArc(s, n);
        }
    }
    
  return s;
}

ListDigraph::Node addSink(ListDigraph& g){

  ListDigraph::Node t = g.addNode();  
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    
        if(countOutArcs(g, n) == 0 && n != t){
            g.addArc(n, t);
        }
    }
    
  return t;
}

void test_overlap(string filename){

	ListDigraph g;
	ListDigraph::NodeMap<int> labels(g);
	ListDigraph::ArcMap<int> weights(g);
	digraphReader(g, "tests/test_graphs/"+filename).nodeMap("label", labels).arcMap("length", weights).run();
	
	Undirector<ListDigraph> undirected(g);
	ListDigraph::NodeMap<int> components(g);
	stronglyConnectedComponents(undirected, components);

	int num_subgraphs = 0;
	for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
		if(components[n] > num_subgraphs) num_subgraphs = components[n];
	}
	num_subgraphs++;
	ListDigraph::NodeMap<ListDigraph::Node> map(g);
	ListDigraph* subgraphs[num_subgraphs];

	for(int i = 0; i < num_subgraphs; i++){
		ListDigraph temp;
		for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
			if(components[n] == i){
				map[n] = temp.addNode();
			}
		}
		for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
			if(components[n] == i){
				for(ListDigraph::OutArcIt o(g, n); o != INVALID; ++o){
					temp.addArc(map[g.source(o)], map[g.target(o)]);
				}
			}
		}
		int num_nodes = countNodes(temp);
		if(num_nodes < 100){
			cout << "subgraph smaller than 100 nodes passed...\n";
			continue;
		}

		cout << "Subgraph of size " << num_nodes << ":\n";

		ListDigraph::Node s = addSource(temp);
		ListDigraph::Node t = addSink(temp);

		//MPC+decomposition

		cout << "MPC + decomposition ";
		clock_t mpc_begin = clock();

		ListDigraph::ArcMap<int> flow(temp);
  		find_minflow_new(temp, flow, s, t);

  		vector<ListDigraph::Node*> decomposition;
  		decompose_graph(temp, flow, s, t, decomposition);

  		clock_t mpc_end = clock();

  		cout << "done in time " << mpc_end-mpc_begin << "\n";

  		//lemon minflow

  		cout << "Lemon minflow ";

  		clock_t minflow_begin = clock();

  		ListDigraph::ArcMap<int> demands(temp, 0);
		for(ListDigraph::NodeIt n(temp); n!= INVALID; ++n){
			ListDigraph::Node new_node = temp.split(n, false);
			ListDigraph::Arc new_edge = temp.addArc(n, new_node);
			demands[new_edge] = 1;
		}

  		ListDigraph::Arc extraArc = temp.addArc(s, t);
  		CostScaling<ListDigraph> costScaling(temp);

  		costScaling.lowerMap(demands);
  		costScaling.costMap(weights);
  		costScaling.stSupply(s, t, 100000);
  		costScaling.run();

  		temp.erase(extraArc);

  		clock_t minflow_end = clock();

  		cout << "done in time " << minflow_end-minflow_begin << "\n\n";

	}

/**
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
	REQUIRE(minflow_value == maxflow_counter);**/
}

int main(){

	test_overlap("test2.graph");
	return 0;
}