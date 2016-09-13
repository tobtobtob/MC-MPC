#ifndef DECOMPOSITION_H_
#define DECOMPOSITION_H_

#include <lemon/list_graph.h>

void solve_MC_MPC(ListDigraph& g, vector<ListDigraph::NodeMap<bool> >& decomposition,
 ListDigraph::ArcMap<int>& weights, ListDigraph::ArcMap<int>& solution);

#endif