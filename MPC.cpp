#include <lemon/list_graph.h>
#include <typeinfo>

#define INFINITE 99999

using namespace lemon;
using namespace std;


void find_feasible_flow(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow){
  
	
	for (ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
	  //iterate through every edge in the graph
	  //if there is some unsatisfied demand, create a flow that satisfies that demand
		if(demands[ai] > flow[ai]){
		
      int newDemand = demands[ai]-flow[ai];
      flow[ai] += newDemand;
			
			//the flow is created by tracking an arbitrary path from the edge to source and sink
      ListDigraph::Node node = g.source(ai);
      
			//when we find a node with no incoming arcs, we have reached the source and the loop will end			
      while(countInArcs(g, node) > 0){
        ListDigraph::InArcIt ia(g, node);
        flow[ia] += newDemand;
        node = g.source(ia);
      }
			
			node = g.target(ai);
			while(countOutArcs(g, node) > 0){
				ListDigraph::OutArcIt oa(g, node);
				flow[oa] += newDemand;
				node = g.target(oa);
			}
			
		}
	}
}

//find minflow by reducing the maximal amount of flow (with maxflow) from a feasible flow
void find_minflow(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, ListDigraph::Node s, ListDigraph::Node t){

  ListDigraph::ArcMap<int> feasible_flow(g);
  find_feasible_flow(g, demands, feasible_flow);
  
  
  ListDigraph copyG;
  
  
  //copy graph
  DigraphCopy<ListDigraph, ListDigraph> cg(g, copyG);
  
  ListDigraph::ArcMap<ListDigraph::Arc> arc_g_to_copy(g);
  cg.arcRef(arc_g_to_copy);
  
  ListDigraph::ArcMap<ListDigraph::Arc> arc_copy_to_g(copyG);
  cg.arcCrossRef(arc_copy_to_g);
  
  ListDigraph::NodeMap<ListDigraph::Node> node_g_to_copy(g);
  cg.nodeRef(node_g_to_copy);
  cg.run();
  
  ListDigraph::Node copyS = node_g_to_copy[s];
  ListDigraph::Node copyT = node_g_to_copy[t];
  
  ListDigraph::ArcMap<int> copy_capacities(copyG);
  
  //set arc capacities in the copy
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    //cout << "HEPPI\n"; 
    copy_capacities[arc_g_to_copy[a]] = INFINITE;
    ListDigraph::Arc backward = copyG.addArc(node_g_to_copy[g.target(a)], node_g_to_copy[g.source(a)]);
    copy_capacities[backward] = feasible_flow[a] - demands[a];
    
  }
  
  //find max-flow in the copy
  
  //
  
}






















