#include "test_utils.h"
#include <lemon/list_graph.h>
#include "../decomposition.h"
#include "../MPC.h"
#include <stdlib.h>
#include <time.h>
#include "catch.hpp"
#include <lemon/bfs.h>

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

    REQUIRE(reachable[labels[s]][labels[c]] == true);
    REQUIRE(reachable[labels[a]][labels[c]] == true);
    REQUIRE(reachable[labels[b]][labels[t]] == true);

    REQUIRE(reachable[labels[c]][labels[s]] == false);
    REQUIRE(reachable[labels[c]][labels[a]] == false);
    REQUIRE(reachable[labels[t]][labels[s]] == false);
}


bool can_reach_some_other_node(int node_id, ListDigraph::Node* ants, int num_ants, ListDigraph& g, Bfs<ListDigraph>& bfs){
  for (int i = 0; i < num_ants; ++i)
  {
    if(node_id == i) continue;
    if(bfs.run(ants[node_id], ants[i])) return true;
  }
  return false;
}

bool isMac(ListDigraph::Node* ants, int num_ants, ListDigraph& g){
  Bfs<ListDigraph> bfs(g);
  for (int i = 0; i < num_ants; ++i)
  {
    if(can_reach_some_other_node(i, ants, num_ants, g, bfs)) return false;
  }
  return true;
}

TEST_CASE("MACs are found in an easy test case")
{
  ListDigraph g;
  ListDigraph::Node s = g.addNode();
  ListDigraph::Node t = g.addNode();
  ListDigraph::Node a = g.addNode();
  ListDigraph::Node b = g.addNode(); 
  ListDigraph::Node c = g.addNode();
  ListDigraph::Node d = g.addNode();

  ListDigraph::Arc sa = g.addArc(s, a);
  ListDigraph::Arc ab = g.addArc(a, b);
  ListDigraph::Arc bt = g.addArc(b, t);
  ListDigraph::Arc sc = g.addArc(s, c);
  ListDigraph::Arc cd = g.addArc(c, d);
  ListDigraph::Arc dt = g.addArc(d, t);

  ListDigraph::NodeMap<int> labels(g);
  int counter = 0;
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    labels[n] = counter;
    counter++;
  }
  ListDigraph::ArcMap<int> demand(g);
  for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
    demand[ai] = 1;
  }
  ListDigraph::ArcMap<int> minflow(g);

  find_minflow(g, demand, minflow, s, t);

  int num_ants = 0;
  for (ListDigraph::OutArcIt o(g, s); o != INVALID; ++o)
  {
    num_ants += minflow[o];
  }

  vector<ListDigraph::Node*> decomposition;

  decompose_graph(g, minflow, s, t, labels, decomposition);

  REQUIRE(!decomposition.empty());

  for(vector<ListDigraph::Node*>::iterator it = decomposition.begin(); it != decomposition.end(); ++it){
    REQUIRE(isMac(*it, num_ants, g));
  }
}

TEST_CASE("MACs are found in a random graph")
{
  srand(time(NULL));
  ListDigraph g;
  ListDigraph::ArcMap<int> demands(g);
  createMACGraph(g, 6, 50, demands);
    
  ListDigraph::Node s, t;
  
  s = addSource(g);
  t = addSink(g);
  ListDigraph::ArcMap<int> minflow(g);
  ListDigraph::NodeMap<int> labels(g);

  int counter = 0;
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    labels[n] = counter;
    counter++;
  }
    
  find_minflow(g, demands, minflow, s, t);
  int num_ants = 0;
  for (ListDigraph::OutArcIt o(g, s); o != INVALID; ++o)
  {
    num_ants += minflow[o];
  }
  vector<ListDigraph::Node*> decomposition;

  decompose_graph(g, minflow, s, t, labels, decomposition);
  REQUIRE(!decomposition.empty());

  for(vector<ListDigraph::Node*>::iterator it = decomposition.begin(); it != decomposition.end(); ++it){
    REQUIRE(isMac(*it, num_ants, g));
  }

}