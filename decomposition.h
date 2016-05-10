#ifndef DECOMPOSITION_H_
#define DECOMPOSITION_H_

#include <lemon/list_graph.h>

using namespace lemon;
using namespace std;

void create_reachability_table(ListDigraph& g, bool** reachability, int num_nodes, ListDigraph::Node source, ListDigraph::NodeMap<int>& labels);

void decompose_graph(ListDigraph& g, ListDigraph::ArcMap<int>& minFlow, ListDigraph::Node s, ListDigraph::Node t, std::vector<ListDigraph::Node*>& decomposition);

vector<pair<ListDigraph::Node, int> >* decompose_graph_new(ListDigraph& g, ListDigraph::ArcMap<int>& minFlow, ListDigraph::Node s, ListDigraph::Node t);

#endif /* DECOMPOSITION_H_ */
