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

	//pushing 100 uniths of flow is just arbitrary number, it has to be equal or bigger than the minimum flow
	costScaling.stSupply(s, t, 100);

	costScaling.run();

	cout << "Total cost: " << costScaling.totalCost() << "\n";

}

int main()
{
	
	ListDigraph test;
	ListDigraph::Node a = test.addNode();
	ListDigraph::Node b = test.addNode();
	ListDigraph::Node c = test.addNode();
	ListDigraph::Node d = test.addNode();
	ListDigraph::Node e = test.addNode();
	ListDigraph::Node f = test.addNode();

	ListDigraph::Arc ac  = test.addArc(a, c);
	ListDigraph::Arc bc  = test.addArc(b, c);
	ListDigraph::Arc cd  = test.addArc(c, d);
	ListDigraph::Arc bf  = test.addArc(b, f);
	ListDigraph::Arc de  = test.addArc(d, e);
	ListDigraph::Arc df  = test.addArc(d, f);

	ListDigraph::ArcMap<int> costs(test);

	costs[ac] = 1;
	costs[bc] = 2;
	costs[cd] = 2;
	costs[bf] = 32;
	costs[de] = 16;
	costs[df] = 8;

	solve_via_minflow(test, costs);

	return 0;
}
