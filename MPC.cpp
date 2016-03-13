#include <lemon/list_graph.h>
#include <lemon/preflow.h>
#include <lemon/adaptors.h>
#include <lemon/cost_scaling.h>
#include <lemon/edmonds_karp.h>
#include <climits>
#include <lemon/connectivity.h>
#include <stack>
#include <utility>
#include <stdlib.h>
#include <fstream>

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


void find_feasible_flow_topsort(ListDigraph& g, ListDigraph::ArcMap<int>& flow, ListDigraph::Node s, ListDigraph::Node t)
{

  int num_nodes = countNodes(g);
  ListDigraph::NodeMap<int> top_order(g);
  topologicalSort(g, top_order);
  ListDigraph::Node top_nodes[num_nodes];

  ListDigraph::NodeMap<bool> covered(g);
  covered[s] = true;
  covered[t] = true;

  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    top_nodes[top_order[n]] = n;
  }

  ListDigraph::Node current;

  while(true){
    //find the node with lowest topsort index
    for(int i = 0; i < num_nodes; i++){
      if(!covered[top_nodes[i]]){
        current = top_nodes[i];
        break;
      }
      //if we hit the last index, all nodes are covered and we can end the function
      if(i == num_nodes-1) return;
    }

    ListDigraph::Node temp = current;
    //track the path back to source, incrementing flow. path chosen doesn't matter
    while(temp != s){
      ListDigraph::InArcIt nextArc(g, temp);
      flow[nextArc]++;
      temp = g.source(nextArc);
    }

    while(current != t){
      int low_index = INT_MAX;
      ListDigraph::Arc nextArc;
      for(ListDigraph::OutArcIt o(g, current); o != INVALID; ++o){
        ListDigraph::Node tempNode = g.target(o);
        if(!covered[tempNode] && top_order[tempNode] < low_index){
          low_index = top_order[tempNode];
          nextArc = o;
        }
      }
      //if next arc still not chosen, choose an arbitrary one. CHANGE to random?
      if(low_index == INT_MAX){
        ListDigraph::OutArcIt oa(g, current);
        nextArc = oa;
      }
      flow[nextArc]++;
      covered[current] = true;
      current = g.target(nextArc);
    }
  }
}

//returns true if an augmenting path is found, false otherwise.  
bool find_augmenting_path(ListDigraph& g, ListDigraph::ArcMap<int>& flow, ListDigraph::ArcMap<int>& demands, ListDigraph::Node s, ListDigraph::Node t)
{
  ListDigraph::NodeMap<bool> visited(g, false);
  visited[s] = true;

  //the boolean value tells if it is a forward arc
  stack<pair<ListDigraph::Arc, bool> > path;
  ListDigraph::Node extraNode = g.addNode();
  ListDigraph::Arc extraArc = g.addArc(extraNode, s);
  visited[extraNode] = true;
  path.push(make_pair(extraArc, true));

  ListDigraph::ArcMap<bool> arcUsed(g, false);
  arcUsed[extraArc] = true;

  loop: while(!path.empty()){

    ListDigraph::Arc currentArc = path.top().first;
    ListDigraph::Node currentNode;
    if(path.top().second){
      currentNode = g.target(currentArc);
    }
    else{
      currentNode = g.source(currentArc);
    }

    visited[currentNode] = true;

    if(currentNode == t){
      while(!path.empty()){
        if(path.top().second){
          flow[path.top().first]--;
        }else{
          flow[path.top().first]++;
        }
        path.pop();
      }

        g.erase(extraArc);
        g.erase(extraNode);
      return true;
    }

    //check the forward arcs for any >1 flow
    for(ListDigraph::OutArcIt o(g, currentNode); o != INVALID; ++o){
      if((flow[o] - demands[o] > 0) && !visited[g.target(o)] && o != currentArc){
        path.push(make_pair(o, true));
        goto loop;
      }
    }
    //and the backward arcs
    for(ListDigraph::InArcIt i(g, currentNode); i != INVALID; ++i){
      if(!visited[g.source(i)] && i != currentArc){
        path.push(make_pair(i, false));
        goto loop;
      }
    }

    path.pop();
  }

  g.erase(extraArc);
  g.erase(extraNode);

}

void find_minflow_new(ListDigraph& g, ListDigraph::ArcMap<int>& flow, ListDigraph::ArcMap<int>& demands, ListDigraph::Node s, ListDigraph::Node t)
{

  find_feasible_flow_topsort(g, flow, s, t);
  while(find_augmenting_path(g, flow, demands, s, t));

}
