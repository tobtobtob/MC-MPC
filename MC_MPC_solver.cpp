#include <lemon/list_graph.h>
#include <stdlib.h>
#include <lemon/adaptors.h>
#include <lemon/cost_scaling.h>
#include <lemon/concepts/graph.h>

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
void solve_MC_MPC(ListDigraph& g, vector<ListDigraph::NodeMap<bool>*> decomposition,
 ListDigraph::ArcMap<int>& weights, ListDigraph::ArcMap<int>& solution){

	 ListDigraph::NodeMap<bool> macNodes(g);

	 //add arcs from source and to sink to every node that is included in some MAC
	 const ListDigraph::Node s, t;
	 for (ListDigraph::NodeIt n(g); n != INVALID; ++n) {
		 if(macNodes[n]){
			 g.addArc(s, n);
			 g.addArc(n, t);
		 }
	 }
	 ListDigraph::ArcMap<bool> allTrueArcs(g, true);
	 ListDigraph::ArcMap<bool> allFalseArcs(g, true);
	 ListDigraph::NodeMap<bool> allTrueNodes(g, true);
	 g.addArc(s, t);

	for (int i = 0; i < decomposition.size(); ++i){

		ListDigraph::NodeMap<bool>* decomp = decomposition[i];
		(*decomp)[s] = true;
		(*decomp)[t] = true;

		SubDigraph<ListDigraph, ListDigraph::NodeMap<bool>, ListDigraph::ArcMap<bool> > part(g, *decomp, allTrueArcs);

		SplitNodes<SubDigraph<ListDigraph, ListDigraph::NodeMap<bool>, ListDigraph::ArcMap<bool> > > splitted(part);
		SplitNodes<SubDigraph<ListDigraph, ListDigraph::NodeMap<bool>, ListDigraph::ArcMap<bool> > >::CombinedArcMap<ListDigraph::ArcMap<bool>, ListDigraph::NodeMap<bool> > demands(allFalseArcs, allTrueNodes);
		
  		CostScaling<SplitNodes<SubDigraph<ListDigraph, ListDigraph::NodeMap<bool>, ListDigraph::ArcMap<bool> > > > costScaling(splitted);
  		costScaling.lowerMap(demands);
  		costScaling.costMap(weights);
		costScaling.stSupply(s, t, 1000);
		costScaling.run();

		for (ListDigraph::ArcIt ai(part); ai != INVALID; ++ai){
			solution[ai] = costScaling.flow(ai);
		}
	 }
}


