#include "test_utils.h"
#include <lemon/list_graph.h>
#include "../decomposition.h"
#include <stdlib.h>
#include <time.h>
#include "catch.hpp"

using namespace std;
using namespace lemon;

TEST_CASE("correct reachability table is created for a simple graph")
{
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

  	int num_nodes = 5;
  	bool** reachable = (bool**) calloc(num_nodes, sizeof(bool*));

  	for(int i=0; i<5; i++){
      reachable[i] = (bool*) calloc(num_nodes, sizeof(bool));
  	}
  	ListDigraph::NodeMap<int> labels(g);
    int counter = 0;
  	for(ListDigraph::NodeIt n(g); n  != INVALID; ++n){
      labels[n] = counter;
      counter++;
    } 

  	create_reachability_table(g,reachable, num_nodes, s, labels);

  	REQUIRE(reachable[labels[s]][labels[t]] == true);
  	REQUIRE(reachable[labels[t]][labels[s]] == false);
}