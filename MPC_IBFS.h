#ifndef MPC_IBFS_H_
#define MPC_IBFS_H_

#include <lemon/list_graph.h>
#include "IBFS/ibfs.h"

using namespace lemon;

void find_minflow_IBFS(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, ListDigraph::NodeMap<int>& labels, ListDigraph::Node s, ListDigraph::Node t);

void find_minflow_ibfs_alt(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow, ListDigraph::NodeMap<int>& labels, ListDigraph::Node s, ListDigraph::Node t, int max_paths);

#endif /* MPC_IBFS_H_ */
