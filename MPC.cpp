#include <lemon/list_graph.h>
#include <typeinfo>

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
			
			int counter = 0;
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

void find_minflow(ListDigraph& g){

}


