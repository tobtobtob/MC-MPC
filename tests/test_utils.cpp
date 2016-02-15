/*
 * test_utils.cpp
 *
 *  Created on: Nov 29, 2015
 *      Author: topi
 */

#include <lemon/list_graph.h>
#include <stdlib.h>
#include <time.h>


using namespace std;
using namespace lemon;

//Simple function for generating acyclic graphs
void createRandomGraph(ListDigraph& g, int num_nodes, float edge_prob){

	srand(time(NULL));
	ListDigraph::NodeMap<int> labels(g);

	for(int i=0; i<num_nodes; i++){
		ListDigraph::Node new_node = g.addNode();
		labels[new_node] = i;
	}
	for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
		for(ListDigraph::NodeIt v(g); v != INVALID; ++v){

			//no edges from bigger nodes to smaller to ensure acyclicity,
			//and no edges from node to itself
			//+ an attempt to create longer graphs
			if(labels[n] >= labels[v] || labels[n] < labels[v]-20) continue;

			if(rand()%100 <= edge_prob*100){
				g.addArc(n, v);
			}
		}
	}
}


void createMACGraph(ListDigraph& g, int num_paths, int path_length, ListDigraph::ArcMap<int>& demands){

	srand(time(NULL));

	ListDigraph::Node prev[num_paths];
	ListDigraph::Node current[num_paths];

	for (int i = 0; i < num_paths; ++i)
	{
		prev[i] = g.addNode();
	}
	for (int i = 0; i < path_length-1; ++i)
	{
		for (int j = 0; j < num_paths; ++j)
		{
			current[j] = g.addNode();
			g.addArc(prev[j], current[j]);
		}
		for (int j = 0; j < num_paths; ++j)
		{
			if(rand()%100 < 50){
				int targetIndex  = j;
				while(targetIndex != j){
					targetIndex = rand()%num_paths;
				}
				g.addArc(prev[j], current[targetIndex]);
			}
		}
		for (int j = 0; j < num_paths; ++j)
		{
			prev[j] = current[j];
		}

	}

	//this splits every node and sets demand for the edge between the halves to 1
	for (ListDigraph::NodeIt n(g); n != INVALID; ++n){
		ListDigraph::Node new_node = g.split(n, false);
		ListDigraph::Arc new_edge = g.addArc(n, new_node);
		demands[new_edge] = 1;
	}

}

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



