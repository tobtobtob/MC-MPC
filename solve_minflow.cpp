#include <lemon/list_graph.h>

using namespace lemon;
using namespace std;

List_Digraph create_minflow_graph(List_Digraph graph, EdgeMap<int> edge_map)
{
	//splits each node into connected nodes, where the first has all the incoming and the second all the outgoing edges
	for (List_Digraph::NodeIt n(g); n != INVALID; ++n)
	{

	}
}