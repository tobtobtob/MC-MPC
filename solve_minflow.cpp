#include <lemon/list_graph.h>

using namespace lemon;
using namespace std;

void transform_to_minflow_graph(List_Digraph g, EdgeMap<int> demands)
{
	//splits each node into connected nodes, where the first has all the incoming and the second all the outgoing edges

	for (List_Digraph::NodeIt n(g); n != INVALID; ++n)
	{
		Node new_node = g.split(n, false);
		Arc new_arc = g.addArc(n, new_node);
		demands[new_arc] = 1;
	}
}

void solve_via_minflow(List_Digraph g)
{
	// transform the graph so that every node is split in two, and demand for the edge between the duplicates is 1

	EdgeMap<int> demands(g);
	transform_to_minflow_graph(g, demands);

	//then add source and tap and connect source to all nodes with no incoming edges and all nodes with no outgoing edges to tap

	Node s = g.addNode();
	Node t = g.addNode();

	for (List_Digraph::NodeIt n(g); n != INVALID; ++n)
	{
		if (countInArcs(g, n) == 0)
			g.addArc(s, n);

		if (countOutArcs(g, n) == 0)
			g.addArc(n, s)
	}


}