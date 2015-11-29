#include <lemon/list_graph.h>

using namespace lemon;
using namespace std;


void find_feasible_flow(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow){

	
	for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
		if(demands[a] > flow[a]){
			flow[a]++;

			ListDigraph::Node node = g.source(a);
			while(countInArcs(g, node) > 0){
				ListDigraph::InArcIt ia(g, node);
				flow[ia]++;
				node = g.source(ia);
			}
			node = g.target(a);
			while(countOutArcs(g, node) > 0){
				ListDigraph::OutArcIt oa(g, node);
				flow[oa]++;
				node = g.source(oa);
			}
		}
	}
}

void find_minflow(ListDigraph& g){

}


