#include <lemon/list_graph.h>
#include <lemon/cost_scaling.h>

using namespace lemon;
using namespace std;

void transform_to_minflow_graph(ListDigraph& g, ListDigraph::ArcMap<int>& demands)
{
	//splits each node into connected nodes, where the first has all the incoming and the second all the outgoing edges

	for (ListDigraph::NodeIt n(g); n != INVALID; ++n)
	{
		ListDigraph::Node new_node = g.split(n, false);
		ListDigraph::Arc new_arc = g.addArc(n, new_node);
		demands[new_arc] = 1;
	}
}

void solve_via_minflow(ListDigraph& g, ListDigraph::ArcMap<int>& costs)
{
	// transform the graph so that every node is split in two, and demand for the edge between the duplicates is 1

	ListDigraph::ArcMap<int> demands(g);
	transform_to_minflow_graph(g, demands);

	//then add source and tap and connect source to all nodes with no incoming edges and all nodes with no outgoing edges to tap

	ListDigraph::Node s = g.addNode();
	ListDigraph::Node t = g.addNode();

	for (ListDigraph::NodeIt n(g); n != INVALID; ++n)
	{
		if (countInArcs(g, n) == 0)
			g.addArc(s, n);

		if (countOutArcs(g, n) == 0)
			g.addArc(n, t);
	}

	//initalize cost scaling algorithm and give it the demands and the costs of the edges

	CostScaling<ListDigraph> costScaling(g);
	costScaling.lowerMap(demands);
	costScaling.costMap(costs);

	//pushing 100 units of flow is just arbitrary number, it has to be equal or bigger than the minimum flow
	costScaling.stSupply(s, t, 100);

	costScaling.run();

	cout << "Total cost: " << costScaling.totalCost() << "\n";

}


