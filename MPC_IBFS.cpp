#include <lemon/list_graph.h>
#include "IBFS/ibfs.h"
#include "MPC.h"

using namespace lemon;
using namespace std;

#define MAX_CAPACITY 99999

//find minflow by reducing the maximal amount of flow (with maxflow) from a feasible flow
void find_minflow_IBFS(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, ListDigraph::NodeMap<int>& labels, ListDigraph::Node s, ListDigraph::Node t)
{

  ListDigraph::ArcMap<int> feasible_flow(g);
  find_feasible_flow(g, demands, feasible_flow);
  //Create IBFS graph and solve maxflow in it

  IBFSGraph* g_ibfs = new IBFSGraph(IBFSGraph::IB_INIT_FAST);

  int num_nodes = countNodes(g);
  int num_arcs = countArcs(g);


  //we need a special labeling for IBFS, because we won't have s and t there
  ListDigraph::Node label_to_node[num_nodes];
  ListDigraph::NodeMap<int> labels_ibfs(g);
  int index_counter = 0;
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    if(n == s || n == t) continue;

    labels_ibfs[n] = index_counter;
    index_counter++;
    label_to_node[labels_ibfs[n]] = n;
  }

  g_ibfs->initSize(num_nodes-2, num_arcs-countOutArcs(g, s) - countInArcs(g, t));
/**
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    if(countInArcs(g, n) == 0){
      g_ibfs->addNode(labels[n], 0, MAX_CAPACITY);
    }
    else if(countOutArcs(g, n) == 0){
      g_ibfs->addNode(labels[n], MAX_CAPACITY, 0);
    }
  }
**/

  ListDigraph::ArcMap<int> arc_labels(g);
  ListDigraph::Arc arc_labels_reverse[num_arcs];

  int counter = 0;
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    arc_labels[a] = counter;
    arc_labels_reverse[counter] = a;
    counter++;
  }

  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    if(g.source(a) == s){
      g_ibfs->addNode(labels_ibfs[g.target(a)], 0, MAX_CAPACITY);
    }
    else if(g.target(a) == t){
      g_ibfs->addNode(labels_ibfs[g.source(a)], MAX_CAPACITY, 0);
    }
    else{
      g_ibfs->addEdge(labels_ibfs[g.target(a)], labels_ibfs[g.source(a)], feasible_flow[a]-demands[a], MAX_CAPACITY, arc_labels[a]);    
    }
  }

  g_ibfs->initGraph();
  g_ibfs->computeMaxFlow();



  while(g_ibfs->arcs != g_ibfs->arcEnd){

    //cout << "LABEL:: " << g_ibfs->arcs->label << "\n";

    if(g_ibfs->arcs->label != -1){

      ListDigraph::Arc a = arc_labels_reverse[g_ibfs->arcs->label];
      int flow_on_arc = MAX_CAPACITY - g_ibfs->arcs->rCap;
      int flow_on_reverse = (feasible_flow[a]-demands[a]) - g_ibfs->arcs->rev->rCap;
      //cout << g_ibfs->arcs->rCap << " " << g_ibfs->arcs->rev->rCap << "\n";
      //cout << feasible_flow[a] << " " << flow_on_arc << " " << flow_on_reverse << " ";
      flow[a] = feasible_flow[a] + flow_on_arc - flow_on_reverse; 
      //cout << flow[a] << "\n";
    }
    g_ibfs->arcs++;
  }

  for(ListDigraph::OutArcIt o(g, s); o != INVALID; ++o){
    for (ListDigraph::OutArcIt oa(g, g.target(o)); oa != INVALID; ++oa)
    {
      flow[o] += flow[oa];
    }
  }

  for(ListDigraph::InArcIt i(g, t); i != INVALID; ++i){
    for(ListDigraph::InArcIt ia(g, g.source(i)); ia != INVALID; ++ia){
      flow[i] += flow[ia];
    }
  }


  
}
