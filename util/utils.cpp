#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

//adds and returns a new source node, which has an outbound arc to all previous source nodes of the graph
ListDigraph::Node add_source(ListDigraph& g){
  ListDigraph::Node s = g.addNode();
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    
        if(countInArcs(g, n) == 0 && n != s){
            g.addArc(s, n);
        }
    }
    
  return s;
}

//adds a sink node so that every sink node in the graph has an outbound arc to the new sink, which is returned
ListDigraph::Node add_sink(ListDigraph& g){

  ListDigraph::Node t = g.addNode();  
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    
        if(countOutArcs(g, n) == 0 && n != t){
            g.addArc(n, t);
        }
    }
    
  return t;
}
