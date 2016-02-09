#include <lemon/list_graph.h>

using namespace lemon;
using namespace std;


#define MAX_NODES 99999


void recur(ListDigraph& g, ListDigraph::Node current, ListDigraph::NodeMap<bool>& visited, bool** reachability, int num_nodes, ListDigraph::NodeMap<int>& labels)
{
	reachability[labels[current]][labels[current]] = true;

	for(ListDigraph::OutArcIt o(g, current); o != INVALID; ++o){

		ListDigraph::Node next = g.target(o);
		reachability[labels[current]][labels[next]] = true;

		if(!visited[next]) recur(g, next, visited, reachability, num_nodes, labels);

		for(int i=0; i<num_nodes; i++){
			reachability[labels[current]][i] = reachability[labels[current]][i] || reachability[labels[next]][i];
		}
	}
	visited[current] = true;
}

void create_reachability_table(ListDigraph& g, bool** reachability, int num_nodes, ListDigraph::Node source, ListDigraph::NodeMap<int>& labels)
{
	ListDigraph::NodeMap<bool> visited(g);
	recur(g, source, visited, reachability, num_nodes, labels);
}

//Decomposition of the graph is modelled by a vector of vectors of nodes. Each vector of nodes forms one antichain, along which the graph can be decomposed. 
void decompose_graph(ListDigraph& g, ListDigraph::ArcMap<int>& minFlow, ListDigraph::Node s, ListDigraph::Node t, ListDigraph::NodeMap<int>& labels)
{
	vector<vector<ListDigraph::Node> > decomposition;

	int num_nodes = countNodes(g);
	bool** reachable = (bool**) calloc(num_nodes, sizeof(bool*));
	for(int i=0; i<num_nodes; i++) reachable[i] = (bool*) calloc(num_nodes, sizeof(bool));

	create_reachability_table(g, reachable, countNodes(g), s, labels);

}