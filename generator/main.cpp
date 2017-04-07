#include <lemon/list_graph.h>
#include <lemon/lgf_writer.h>
#include <lemon/dfs.h>
#include "../util/utils.h"
#include <string.h>
#include <time.h>

using namespace lemon;
using namespace std;

#define MAX_WEIGHT 1000

void createKPathGraph(string output_filename, int k, int n, int m);

int main(int argc, char* argv[])
{
  if (argc != 5){
    cerr << "Usage: " << argv[0] << " OUTPUT_FILENAME NUM_PATHS PATH_LENGTH NUM_EXTRA_ARCS" << endl;
    return 1;
  }
  int num_paths, path_length;
  float arc_prob;
  try {
    num_paths = stoi(argv[2]);
    path_length = stoi(argv[3]);
    arc_prob = stoi(argv[4]);
  } catch (const std::exception& e) {
    cerr << "ERROR: error reading numerical parameters\n";
    return 1;
  }

  createKPathGraph(argv[1], num_paths, path_length, arc_prob);

  return 0;
}

void createKPathGraph(string output_filename, int k, int n, int m)
{
	srand(time(NULL));

  ListDigraph g;
  ListDigraph::ArcMap<int> arc_weights(g);
  ListDigraph::ArcMap<int> arc_labels(g);
  ListDigraph::NodeMap<int> node_labels(g);

	ListDigraph::Node* nodes[k];
	for (int i = 0; i < k; ++i){
		nodes[i] = (ListDigraph::Node*) calloc(n, sizeof(ListDigraph::Node));
	}
  int arc_label_index = 0;
  int node_label_index = 0;
	for (int i = 0; i < k; ++i){
		for (int j = 0; j < n; ++j){
			nodes[i][j] = g.addNode();
      node_labels[nodes[i][j]] = node_label_index;
      node_label_index++;
			if(j != 0){
        ListDigraph::Arc temp_arc = g.addArc(nodes[i][j-1], nodes[i][j]);
        arc_labels[temp_arc] = arc_label_index;
        arc_label_index++;
      }
		}
	}

  Dfs<ListDigraph> dfs(g);

	for (int i = 0; i < m; ++i){
		int k1 = rand()%k;
		int k2 = rand()%k;
		int n1 = rand()%n;
		int n2 = rand()%n;

    ListDigraph::Node from_node = nodes[k1][n1];
    ListDigraph::Node to_node = nodes[k2][n2];

		if(findArc(g, from_node, to_node) == INVALID && !dfs.run(to_node, from_node)){
			ListDigraph::Arc temp_arc = g.addArc(nodes[k1][n1], nodes[k2][n2]);
      arc_labels[temp_arc] = arc_label_index;
      arc_label_index++;
		}
	}
	for (ListDigraph::ArcIt a(g); a != INVALID; ++a){
		arc_weights[a] = rand()%MAX_WEIGHT;
	}

  DigraphWriter<ListDigraph>(g, output_filename)
    .nodeMap("label", node_labels)
    .arcMap("label", arc_labels)
    .arcMap("weight", arc_weights)
    .run();
}


