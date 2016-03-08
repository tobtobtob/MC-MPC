#include <lemon/list_graph.h>
#include <lemon/preflow.h>
#include <lemon/adaptors.h>
#include <lemon/cost_scaling.h>
#include <lemon/edmonds_karp.h>

#define INFINITE 20

using namespace lemon;
using namespace std;


void find_feasible_flow(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow)
{ 
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
void find_minflow(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, ListDigraph::Node s, ListDigraph::Node t)
{

  clock_t begin_time = clock();
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
  
  //link arcs of original graph g to the reverse arcs in copy, for later use.
  ListDigraph::ArcMap<ListDigraph::Arc> reverse_arc(g);
  
  //set arc capacities in the copy
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
  
    copy_capacities[arc_g_to_copy[a]] = INFINITE;
    ListDigraph::Arc backward = copyG.addArc(node_g_to_copy[g.target(a)], node_g_to_copy[g.source(a)]);
    copy_capacities[backward] = feasible_flow[a] - demands[a];
    
    reverse_arc[a] = backward;
  }
  
  //find max-flow in the copy

  
  Preflow<ListDigraph, ListDigraph::ArcMap<int> > preflow(copyG, copy_capacities, copyT, copyS);
  
  preflow.run();

  
  //then calculate the final flow
  
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    flow[a] = feasible_flow[a] - preflow.flow(reverse_arc[a]) + preflow.flow(arc_g_to_copy[a]);
  }
  
}

void find_feasible_flow_alt(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, int max_paths)
{ 
  for (ListDigraph::ArcIt a(g); a != INVALID; ++a)
  {
    if(flow[a] < max_paths){
      flow[a] += max_paths;
      //the flow is created by tracking an arbitrary path from the edge to source and sink
      ListDigraph::Node node = g.source(a);
      
      //when we find a node with no incoming arcs, we have reached the source and the loop will end     
      while(countInArcs(g, node) > 0){
        ListDigraph::Arc tempArc;
        bool found = false;
        for(ListDigraph::InArcIt ia(g, node); ia != INVALID; ++ia){
          if(flow[ia] < max_paths){
            tempArc = ia;
            found = true;
            break;
          }
        }
        if(!found){
          ListDigraph::InArcIt tempIa(g, node);
          tempArc = tempIa;
        }
        flow[tempArc] += max_paths;
        node = g.source(tempArc);
      }
      
      node = g.target(a);
      while(countOutArcs(g, node) > 0){
        ListDigraph::Arc tempArc;
        bool found = false;
        for(ListDigraph::OutArcIt oa(g, node); oa != INVALID; ++oa){
          if(flow[oa] < max_paths){
            tempArc = oa;
            found = true;
            break;
          }
        }
        if(!found){
          ListDigraph::OutArcIt tempOa(g, node);
          tempArc = tempOa;
        }
        flow[tempArc] += max_paths;
        node = g.target(tempArc);
      }
    }
  }
}

//find minflow by reducing the maximal amount of flow (with maxflow) from a feasible flow
void find_minflow_alt(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, ListDigraph::Node s, ListDigraph::Node t, int max_paths)
{

  clock_t begin_time = clock();
  ListDigraph::ArcMap<int> feasible_flow(g);
  find_feasible_flow_alt(g, demands, feasible_flow, max_paths);
  
  ListDigraph::ArcMap<int> capacities(g);
  for (ListDigraph::ArcIt a(g); a != INVALID; ++a)
  {
    capacities[a] = feasible_flow[a] - demands[a];
  }

  Preflow<ListDigraph, ListDigraph::ArcMap<int> > edmonds(g, capacities, s, t);
  
  edmonds.run();

  
  //then calculate the final flow
  
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    flow[a] = feasible_flow[a] - edmonds.flow(a);
  }
  
}