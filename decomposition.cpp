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


bool can_reach_another_node(int ant_index, bool** reachable, ListDigraph::NodeMap<int>& labels, ListDigraph::Node* ants, int num_ants)
{
	for(int i = 0; i< num_ants; i++){
		if(i == ant_index) continue;
		if(reachable[labels[ants[ant_index]]][labels[ants[i]]]) return true;
	}
	return false;
}

bool is_MAC(bool** reachable, ListDigraph::NodeMap<int>& labels, ListDigraph::Node* ants, int num_ants)
{
	for(int i = 0; i< num_ants; i++){
		if(can_reach_another_node(i, reachable, labels, ants, num_ants)) return false;
	}
	return true;
}

//returns a node that is one step forwards from the given node
ListDigraph::Node get_ant_move(ListDigraph::Node node, ListDigraph::ArcMap<int>& minFlow, ListDigraph& g)
{
	for(ListDigraph::OutArcIt o(g, node); o != INVALID; ++o){
		if(minFlow[o] > 0){
			//mark the path used -- reduce the flow by one
			minFlow[o] -= 1;
			return g.target(o);
		}
	}
	//if no next node is found, we return the given node. In this case the given node should be the sink
	return node;
}

//Decomposition of the graph is modelled by a vector of vectors of nodes. Each vector of nodes forms one antichain, along which the graph can be decomposed. 
void decompose_graph(ListDigraph& g, ListDigraph::ArcMap<int>& minFlow, ListDigraph::Node s, ListDigraph::Node t, ListDigraph::NodeMap<int>& labels, vector<ListDigraph::Node*>& decomposition)
{

	int num_nodes = countNodes(g);
	bool** reachable = (bool**) calloc(num_nodes, sizeof(bool*));
	for(int i=0; i<num_nodes; i++) reachable[i] = (bool*) calloc(num_nodes, sizeof(bool));

	create_reachability_table(g, reachable, countNodes(g), s, labels);

	int num_ants = 0;
	for(ListDigraph::OutArcIt o(g, s); o != INVALID; ++o){
		num_ants += minFlow[o];
	}

	//init the array of "ants" and set all ants to source
	ListDigraph::Node ants[num_ants];
	for(int i = 0; i< num_ants; i++){
		ants[i] = s;
	}

	bool exit_loop = false;
	while(!exit_loop){
		//if one of the ants points to the sink, no more MACs can be found

		if(is_MAC(reachable, labels, ants, num_ants)){
			//copy the current ant setup to the decomposition vector
			ListDigraph::Node* new_MAC = (ListDigraph::Node*) calloc(num_ants, sizeof(ListDigraph::Node));
			copy(ants, ants+num_ants, new_MAC);
			decomposition.push_back(new_MAC);

			//then move each ant by one
			for (int i = 0; i < num_ants; ++i){
				ants[i] = get_ant_move(ants[i], minFlow, g);
			}
		}
		else{
			for (int i = 0; i < num_ants; ++i){
				//we move each ant until it can't reach any other ant. after this we might have a MAC
				while(can_reach_another_node(i, reachable, labels, ants, num_ants)){;
					ants[i] = get_ant_move(ants[i], minFlow, g);

					//without this it would eventually get stuck
					if(ants[i] == t){
						break;
					}
				}
			}
		}
		for (int i = 0; i < num_ants; ++i){
			if(ants[i] == t) exit_loop = true;
		}

	}
}
