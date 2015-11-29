

#ifndef SOLVE_MINFLOW_H_
#define SOLVE_MINFLOW_H_

#include <lemon/list_graph.h>

using namespace lemon;

void transform_to_minflow_graph(ListDigraph& g, ListDigraph::ArcMap<int>& demands);

void solve_via_minflow(ListDigraph& g, ListDigraph::ArcMap<int>& costs);


#endif /* SOLVE_MINFLOW_H_ */
