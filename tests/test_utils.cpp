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
	ListDigraph::ArcMap<int> labels(g);

	for(int i=0; i<num_nodes; i++){
		ListDigraph::Node new_node = g.addNode();
		labels[new_node] = i;
	}
	for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
		for(ListDigraph::NodeIt v(g); v != INVALID; ++v){

			//no edges from bigger nodes to smaller to ensure acyclicity,
			//and no edges from node to itself
			//+ an attempt to create longer graphs
			if(labels[n] >= labels[v] || labels[n] < labels[v]-10) continue;

			if(rand()%100 <= edge_prob*100){
				g.addArc(n, v);
			}
		}
	}
}

//adds a source node and a sink node to the given graph
void addSourceAndSink(ListDigraph& g){

    ListDigraph::Node s = g.addNode();
    ListDigraph::Node t = g.addNode();

    for(ListDigraph::NodeIt n; n != INVALID; ++n){
        if(countInArcs(g, n) == 0){
            g.addArc(s, n);
        }
        if(countOutArcs(g, n) == 0){
            g.addArc(n, t);
        }
    }


}



