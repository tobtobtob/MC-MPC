#include <lemon/list_graph.h>

using namespace lemon;
using namespace std;


#define MAX_NODES 99999

void create_reachability_table(ListDigraph& g, bool reachability[][], int num_nodes)
{

}

void recur(ListDigraph& g, ListDigraph::Node current, bool visited[], bool reachability[][], int num_nodes)
{

	for(ListDigraph::OutArcIt o(g, current); o != INVALID; ++o){

		ListDigraph::Node next = g.source(current);
		if(!visited[g.id(next)]) recur(g, next, visited, reachability);
		for(int i=0; i<)
	}
}

//Decomposition of the graph is modelled by a vector of vectors of nodes. Each vector of nodes forms one antichain, along which the graph can be decomposed. 
vector<vector<ListDigraph::Node> >& decompose_graph(ListDigraph& g, ListDigraph::ArcMap<int>& minFlow, ListDigraph::Node s, ListDigraph::Node t)
{
	vector<vector<ListDigraph::Node> > decomposition;

	bool reachable[MAX_NODES][MAX_NODES];


	return decomposition;
}