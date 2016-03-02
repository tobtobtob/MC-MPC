
#ifndef TESTS_TEST_UTILS_H_
#define TESTS_TEST_UTILS_H_

#include <lemon/list_graph.h>
#include <lemon/bfs.h>


using namespace lemon;


void createRandomGraph(ListDigraph& g, int num_nodes, float edge_prob);

ListDigraph::Node addSource(ListDigraph& g);

ListDigraph::Node addSink(ListDigraph& g);

void createMACGraph(ListDigraph& g, int num_paths, int path_length, ListDigraph::ArcMap<int>& demands);

void createKPathGraph(ListDigraph& g, int k, int n, int m, ListDigraph::ArcMap<int>& weights, ListDigraph::ArcMap<int>& demands);

bool can_reach_some_other_node(int node_id, ListDigraph::Node* ants, int num_ants, ListDigraph& g, Bfs<ListDigraph>& bfs);

bool isMac(ListDigraph::Node* ants, int num_ants, ListDigraph& g);

void flow_satisfies_demands(ListDigraph& g, ListDigraph::ArcMap<int>& flow, ListDigraph::ArcMap<int>& demand);

void check_flow_conservation(ListDigraph& g, ListDigraph::ArcMap<int>& flow);

void drawGraphToFile(ListDigraph& g);

void drawGraphToFileWithArcMap(ListDigraph& g, ListDigraph::ArcMap<int>& map);

#endif /* TESTS_TEST_UTILS_H_ */
