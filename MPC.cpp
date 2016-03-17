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

void find_feasible_flow_most_uncovered(ListDigraph& g, ListDigraph::ArcMap<int>& flow, ListDigraph::Node s, ListDigraph::Node t){
  int num_nodes = countNodes(g);
  ListDigraph::NodeMap<int> top_order(g);
  topologicalSort(g, top_order);
  ListDigraph::Node top_nodes[num_nodes];

  ListDigraph::NodeMap<bool> covered(g);
  covered[s] = true;
  covered[t] = true;

  //top nodes will have nodes in descending topological order
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    top_nodes[num_nodes-top_order[n]-1] = n;
  }

  //this node map tells us the maximum number of uncovered nodes reachable from this node via single path
  ListDigraph::NodeMap<int> uncovered(g, 0);

  while(true){

    //update uncovered
    for(int i = 0; i < num_nodes; i++){
      ListDigraph::Node temp = top_nodes[i];
      uncovered[temp] = 0;
      for (ListDigraph::OutArcIt o(g, temp); o != INVALID; ++o)
      {
       uncovered[temp] = max(uncovered[g.target(o)], uncovered[temp]);
      }
      if(!covered[temp]){
        uncovered[temp]++;
      }
    }

    ListDigraph::Node current = s;
    while(current != t){
      int max_uncovered = 0;
      ListDigraph::Arc nextArc;
      for(ListDigraph::OutArcIt o(g, current); o != INVALID; ++o){
        if(uncovered[g.target(o)] > max_uncovered){
          nextArc = o;
          max_uncovered = uncovered[g.target(o)];
        }
      }
      //if there is no uncovered nodes at the source, every node is covered and we are ready
      //else we just choose an arbitrary edge (there are no reachable uncovered nodes)
      if(max_uncovered == 0){
        if(current == s) return;
        ListDigraph::OutArcIt o(g, current);
        nextArc = o;
      }

      flow[nextArc]++;
      current = g.target(nextArc);
      covered[current] = true;
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
  return false;
}

//returns true if an augmenting path is found, false otherwise. This uses no demands on arcs
bool find_augmenting_path_no_demands(ListDigraph& g, ListDigraph::ArcMap<int>& flow, ListDigraph::Node s, ListDigraph::Node t)
{

  ListDigraph::NodeMap<bool> visited(g, false);
  visited[s] = true;

  //the boolean value tells if it is a forward arc
  stack<pair<ListDigraph::Arc, bool> > path;
  ListDigraph::Node extraNode = g.addNode();
  ListDigraph::Arc extraArc = g.addArc(extraNode, s);
  visited[extraNode] = true;
  path.push(make_pair(extraArc, true));

  ListDigraph::NodeMap<int> flow_on_node(g);
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    for(ListDigraph::OutArcIt o(g, n); o != INVALID; ++o){
      flow_on_node[n] += flow[o];
    }
  }

  ListDigraph::ArcMap<pair<bool, bool> > visitedArc(g, make_pair(false, false));

  loop: while(!path.empty()){
    //cout << "poop\n";

    ListDigraph::Arc currentArc = path.top().first;
    ListDigraph::Node currentNode;
    if(path.top().second){
      currentNode = g.target(currentArc);
      visitedArc[currentArc].first = true;
    }
    else{
      currentNode = g.source(currentArc);
      visitedArc[currentArc].second = true;
    }

    visited[currentNode] = true;

    if(currentNode == t){
      while(!path.empty()){
        if(path.top().second){
          
          flow_on_node[g.source(path.top().first)]--;
          flow[path.top().first]--;
        }else{
          flow[path.top().first]++;
          flow_on_node[g.source(path.top().first)]++;
        }
        path.pop();
      }

        g.erase(extraArc);
        g.erase(extraNode);
      return true;
    }

    //we can use forward arcs only if there is extra flow on that node
    if(flow_on_node[currentNode] > 1 || !path.top().second){

      for(ListDigraph::OutArcIt o(g, currentNode); o != INVALID; ++o){
        if(flow[o] > 0 && !visitedArc[o].first && o != currentArc){
          path.push(make_pair(o, true));
          goto loop;
        }
      }
    }
    //and the backward arcs
    for(ListDigraph::InArcIt i(g, currentNode); i != INVALID; ++i){
      if(!visitedArc[i].second && i != currentArc){
        path.push(make_pair(i, false));
        goto loop;
      }
    }

    path.pop();
  }

  g.erase(extraArc);
  g.erase(extraNode);
  return false;
}

void find_minflow_new(ListDigraph& g, ListDigraph::ArcMap<int>& flow, ListDigraph::Node s, ListDigraph::Node t)
{

  find_feasible_flow_most_uncovered(g, flow, s, t);
  while(find_augmenting_path_no_demands(g, flow, s, t)){   
  }
}
