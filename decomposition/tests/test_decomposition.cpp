#include "../../tests/test_utils.h"
#include <lemon/list_graph.h>
#include "../decomposition.h"
#include "../MPC.h"
#include <stdlib.h>
#include <time.h>
#include "../../tests/catch.hpp"
#include <lemon/bfs.h>

using namespace std;
using namespace lemon;


//these tests assume that decomposer returns a list of MACs
// TODO: fix these
/**
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

  find_minflow(g, minflow, s, t);

  int num_ants = 0;
  for (ListDigraph::OutArcIt o(g, s); o != INVALID; ++o)
  {
    num_ants += minflow[o];
  }

  vector<ListDigraph::Node*> decomposition;

  decompose_graph(g, minflow, s, t, decomposition);

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
    
  find_minflow(g, minflow, s, t);
  int num_ants = 0;
  for (ListDigraph::OutArcIt o(g, s); o != INVALID; ++o)
  {
    num_ants += minflow[o];
  }
  vector<ListDigraph::Node*> decomposition;

  decompose_graph(g, minflow, s, t, decomposition);
  REQUIRE(!decomposition.empty());

  for(vector<ListDigraph::Node*>::iterator it = decomposition.begin(); it != decomposition.end(); ++it){
    REQUIRE(isMac(*it, num_ants, g));
  }

}**/