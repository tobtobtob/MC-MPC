#include <lemon/list_graph.h>
#include "solve_minflow.h"
#include "MPC.h"


using namespace lemon;
using namespace std;

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



