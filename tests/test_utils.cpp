#include <lemon/list_graph.h>
#include <stdlib.h>
#include <time.h>
#include <lemon/bfs.h>
#include "catch.hpp"
#include <iostream>
#include <fstream>
#include <lemon/adaptors.h>
#include <lemon/connectivity.h>
#include <vector>



using namespace std;
using namespace lemon;

void drawGraphToFile(ListDigraph& g){
	ofstream myfile;
	myfile.open("graph.dot");
	myfile << "digraph g {\n";
	for (ListDigraph::ArcIt a(g); a!= INVALID; ++a)
	{
		myfile << g.id(g.source(a)) << " -> " << g.id(g.target(a)) <<  "\n";
	}
	myfile << "}\n";
	myfile.close();
}

void drawGraphToFileWithArcMap(ListDigraph& g, ListDigraph::ArcMap<int>& map){
	ofstream myfile;
	myfile.open("graph.dot");
	myfile << "digraph g {\n";
	for (ListDigraph::ArcIt a(g); a!= INVALID; ++a)
	{
		myfile << g.id(g.source(a)) << " -> " << g.id(g.target(a)) << " [label=\"" << map[a] << "\"] \n";
	}
	myfile << "}\n";
	myfile.close();
}

//Simple function for generating acyclic graphs
void createRandomGraph(ListDigraph& g, int num_nodes, float edge_prob){

	srand(time(NULL));
	ListDigraph::NodeMap<int> labels(g);

	for(int i=0; i<num_nodes; i++){
		ListDigraph::Node new_node = g.addNode();
		labels[new_node] = i;
	}
	for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
		for(ListDigraph::NodeIt v(g); v != INVALID; ++v){

			//no edges from bigger nodes to smaller to ensure acyclicity,
			//and no edges from node to itself
			//+ an attempt to create longer graphs
			if(labels[n] >= labels[v] || labels[n] < labels[v]-20) continue;

			if(rand()%100 <= edge_prob*100){
				g.addArc(n, v);
			}
		}
	}
}

void createKPathGraph(ListDigraph& g, int k, int n, int m, ListDigraph::ArcMap<int>& weights, ListDigraph::ArcMap<int>& demands){
	srand(time(NULL));

	ListDigraph::Node* nodes[k];
	for (int i = 0; i < k; ++i)
	{
		nodes[i] = (ListDigraph::Node*) calloc(n, sizeof(ListDigraph::Node));
	}
	for (int i = 0; i < k; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			nodes[i][j] = g.addNode();
			if(j != 0) g.addArc(nodes[i][j-1], nodes[i][j]);
		}
	}
	for (int i = 0; i < m; ++i)
	{
		int k1 = rand()%k;
		int k2 = rand()%k;
		int n1 = rand()%(n-1);
		int n2 = (rand()%(n-n1-1))+n1+1;

		if(findArc(g, nodes[k1][n1], nodes[k2][n2]) == INVALID){
			g.addArc(nodes[k1][n1], nodes[k2][n2]);
		}
	}
	for (ListDigraph::ArcIt a(g); a != INVALID; ++a)
	{
		weights[a] = rand()%1000;
	}
	
	//this splits every node and sets demand for the edge between the halves to 1
	for (ListDigraph::NodeIt n(g); n != INVALID; ++n){
		ListDigraph::Node new_node = g.split(n, false);
		ListDigraph::Arc new_edge = g.addArc(n, new_node);
		demands[new_edge] = 1;
	}
	

}


void createMACGraph(ListDigraph& g, int num_paths, int path_length, ListDigraph::ArcMap<int>& demands){

	srand(time(NULL));

	ListDigraph::Node *prev = new ListDigraph::Node[num_paths];
	ListDigraph::Node *current = new ListDigraph::Node[num_paths];

	for (int i = 0; i < num_paths; ++i)
	{
		prev[i] = g.addNode();
	}
	for (int i = 0; i < path_length-1; ++i)
	{
		for (int j = 0; j < num_paths; ++j)
		{
			current[j] = g.addNode();
			g.addArc(prev[j], current[j]);
		}
		for (int j = 0; j < num_paths; ++j)
		{
			if(rand()%100 < 50){
				int targetIndex  = j;
				while(targetIndex != j){
					targetIndex = rand()%num_paths;
				}
				g.addArc(prev[j], current[targetIndex]);
			}
		}
		for (int j = 0; j < num_paths; ++j)
		{
			prev[j] = current[j];
		}

	}

	//this splits every node and sets demand for the edge between the halves to 1
	for (ListDigraph::NodeIt n(g); n != INVALID; ++n){
		ListDigraph::Node new_node = g.split(n, false);
		ListDigraph::Arc new_edge = g.addArc(n, new_node);
		demands[new_edge] = 1;
	}

}

ListDigraph::Node addSource(ListDigraph& g){

  ListDigraph::Node s = g.addNode();
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    
        if(countInArcs(g, n) == 0 && n != s){
            g.addArc(s, n);
        }
    }
    
  return s;
}

ListDigraph::Node addSink(ListDigraph& g){

  ListDigraph::Node t = g.addNode();  
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    
        if(countOutArcs(g, n) == 0 && n != t){
            g.addArc(n, t);
        }
    }
    
  return t;
}


bool can_reach_some_other_node(int node_id, ListDigraph::Node* ants, int num_ants, ListDigraph& g, Bfs<ListDigraph>& bfs){
  for (int i = 0; i < num_ants; ++i)
  {
    if(node_id == i) continue;
    if(bfs.run(ants[node_id], ants[i])) return true;
  }
  return false;
}

bool isMac(ListDigraph::Node* ants, int num_ants, ListDigraph& g){
  Bfs<ListDigraph> bfs(g);
  for (int i = 0; i < num_ants; ++i)
  {
    if(can_reach_some_other_node(i, ants, num_ants, g, bfs)) return false;
  }
  return true;
}

void flow_satisfies_demands(ListDigraph& g, ListDigraph::ArcMap<int>& flow, ListDigraph::ArcMap<int>& demand){
  int f, d;
  for(ListDigraph::ArcIt ai(g); ai != INVALID; ++ai){
    f = flow[ai];
    d = demand[ai];
    REQUIRE(f >= d);
  }
}

void check_flow_conservation(ListDigraph& g, ListDigraph::ArcMap<int>& flow){
  //check for flow conservation; each node (Except s and t) receives and sends the same amount of flow
  for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
    int inflow = 0;
    int outflow = 0;

    //skip s and t
    if(countInArcs(g, n) == 0 || countOutArcs(g, n) == 0){
    	continue;
    }

    for(ListDigraph::InArcIt ia(g, n); ia != INVALID; ++ia){
    	inflow += flow[ia];
    }
    for(ListDigraph::OutArcIt oa(g, n); oa != INVALID; ++oa){
    	outflow += flow[oa];
    }

    REQUIRE(inflow == outflow);
  }
}

//splits graph into connected components
void split_graph(ListDigraph& g, vector<ListDigraph*>& graphs){

	Undirector<ListDigraph> undirected(g);
	ListDigraph::NodeMap<int> components(g);
	stronglyConnectedComponents(undirected, components);

	int num_subgraphs = 0;
	for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
		if(components[n] > num_subgraphs) num_subgraphs = components[n];
	}
	num_subgraphs++;
	ListDigraph::NodeMap<ListDigraph::Node> map(g);

	for(int i = 0; i < num_subgraphs; i++){
		ListDigraph temp;
		for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
			if(components[n] == i){
				map[n] = temp.addNode();
			}
		}
		for(ListDigraph::NodeIt n(g); n != INVALID; ++n){
			if(components[n] == i){
				for(ListDigraph::OutArcIt o(g, n); o != INVALID; ++o){
					temp.addArc(map[g.source(o)], map[g.target(o)]);
				}
			}
		}
		graphs.push_back(&temp);
	}
}