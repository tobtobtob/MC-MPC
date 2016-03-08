#ifndef MPC_H_
#define MPC_H_

#include <lemon/list_graph.h>

using namespace lemon;

void find_feasible_flow(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow);

void find_minflow(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, ListDigraph::Node s, ListDigraph::Node t);

void find_feasible_flow_alt(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, int max_paths);

void find_minflow_alt(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, ListDigraph::Node s, ListDigraph::Node t, int max_paths);

#endif /* MPC_H_ */
