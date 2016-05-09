#include <lemon/list_graph.h>
#include <time.h>
#include <stack>
#include <climits>

using namespace lemon;
using namespace std;


bool can_reach_another_node(int path_index, ListDigraph::NodeMap<int*>& reachable, vector<ListDigraph::Node>* paths, int* path_indices, int num_paths)
{
	for(int i = 0; i< num_paths; i++){
		if(i == path_index) continue;
		if(reachable[paths[path_index][path_indices[path_index]]][i] <= path_indices[i]) return true;
	}
	return false;
}

bool is_MAC(ListDigraph::NodeMap<int*>& reachable, vector<ListDigraph::Node>* paths, int* path_indices, int num_paths)
{
	for(int i = 0; i< num_paths; i++){
		if(can_reach_another_node(i, reachable, paths, path_indices, num_paths)) return false;
	}
	return true;
}

void decompose_graph(ListDigraph& g, ListDigraph::ArcMap<int>& minFlow, ListDigraph::Node s, ListDigraph::Node t, vector<ListDigraph::Node*>& decomposition){

	int num_paths = 0;
	for(ListDigraph::OutArcIt o(g, s); o != INVALID; ++o){
		num_paths += minFlow[o];
	}

	//Extract paths from the graph according to the minflow and create reachability tables

	//vector<ListDigraph::Node> paths[num_paths];
	vector<ListDigraph::Node>* paths = new vector<ListDigraph::Node>[num_paths];
	ListDigraph::NodeMap<int*> reachable(g);

	for (int i = 0; i < num_paths; ++i)
	{
		ListDigraph::Node node = s;
		int path_index = 0;
		while(node != t){
			if(reachable[node] == NULL){
				reachable[node] = (int*) calloc(num_paths, sizeof(int));
				for (int i = 0; i < num_paths; ++i)
				{
					reachable[node][i] = INT_MAX;
				}
			}
			reachable[node][i] = path_index;

			for (ListDigraph::OutArcIt o(g, node); o != INVALID; ++o)
			{
				if(minFlow[o] > 0){
					minFlow[o] -= 1;
					paths[i].push_back(node);
					node = g.target(o);
					break;
				}
			}
			path_index++;
		}

		if(reachable[t] == NULL){
			reachable[t] = (int*) calloc(num_paths, sizeof(int));
		}
		reachable[t][i] = path_index;
		paths[i].push_back(t);
	}

	//DFS

	stack<ListDigraph::Node> node_stack;
	node_stack.push(s);
	ListDigraph::NodeMap<bool> visited(g);

	loop: while(!node_stack.empty()){
		ListDigraph::Node node = node_stack.top();

		for(ListDigraph::OutArcIt o(g, node); o != INVALID; ++o){
			ListDigraph::Node child = g.target(o);
			if(!visited[child]){
				node_stack.push(child);
				goto loop;
			}
		}

		for(ListDigraph::OutArcIt o(g, node); o != INVALID; ++o){
			ListDigraph::Node child = g.target(o);
			for (int i = 0; i < num_paths; ++i)
			{
				reachable[node][i] = min(reachable[node][i], reachable[child][i]);
			}
		}	
		visited[node] = true;
		node_stack.pop();
	}

	//create decomposition

	int path_indices[num_paths];
	for (int i = 0; i < num_paths; ++i) path_indices[i] = 0;

	bool exit_loop = false;

	while(!exit_loop){

		if(is_MAC(reachable, paths, path_indices, num_paths)){
			ListDigraph::Node* new_MAC = (ListDigraph::Node*) calloc(num_paths, sizeof(ListDigraph::Node));
			for (int i = 0; i < num_paths; ++i)
			{
				 new_MAC[i] = paths[i][path_indices[i]];
			}
			decomposition.push_back(new_MAC);

			int lowest_index = INT_MAX;
			for (int i = 0; i < num_paths; ++i)
			{
				if(path_indices[i] < lowest_index){
					lowest_index = i;
				}
			}
			path_indices[lowest_index]++;
		}

		else{
			for (int i = 0; i < num_paths; ++i)
			{
				while(can_reach_another_node(i, reachable, paths, path_indices, num_paths) && paths[i][path_indices[i]] != t){
					path_indices[i]++;
				}
			}
		}

		for (int i = 0; i < num_paths; ++i)
		{
			if(paths[i][path_indices[i]] == t){
				exit_loop = true;
			}
		}
	}


}