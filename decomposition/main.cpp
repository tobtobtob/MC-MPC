#include <lemon/list_graph.h>
#include "decomposition.h"
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include "MPC.h"
#include "../util/utils.h"
#include <string.h>

using namespace lemon;
using namespace std;

int main()
{

  string filename = "example_graph.txt";
  ListDigraph graph;

  ListDigraph::NodeMap<int> node_labels(graph);
  ListDigraph::ArcMap<int> arc_labels(graph);
  ListDigraph::ArcMap<int> arc_weights(graph);

  digraphReader(graph, filename)
    .nodeMap("label", node_labels)
    .arcMap("label", arc_labels)
    .arcMap("weight", arc_weights)
    .run();
  
  ListDigraph::Node s = add_source(graph);
  ListDigraph::Node t = add_sink(graph);

  ListDigraph::ArcMap<int> minflow(graph);
  find_minflow(graph, minflow, s, t);

  ListDigraph::ArcMap<int> decomposition(graph);
  decompose_graph(graph, minflow, s, t, decomposition);

  //sink and source nodes added to the graph should not be included in the output
  graph.erase(s);
  graph.erase(t);

  int index = 1;
  bool terminate_loop = false;
  //TODO fix this
  int max_loops = 10;
  while(true)
  {
    if(index > max_loops) break;
    terminate_loop = true;

    ListDigraph temp;
    ListDigraph::NodeMap<int> temp_node_labels(temp);
    ListDigraph::ArcMap<int> temp_arc_labels(temp);
    ListDigraph::ArcMap<int> temp_arc_weights(temp);

    //mapping from original graph to a decomposed part
    ListDigraph::Node null_node;
    ListDigraph::NodeMap<ListDigraph::Node> mapping(graph, null_node);

    for(ListDigraph::ArcIt a(graph); a != INVALID; ++a){
      if(decomposition[a] == index){
        terminate_loop = false;
        ListDigraph::Node source = graph.source(a);
        ListDigraph::Node target = graph.target(a);
        if(mapping[source] == null_node){
          mapping[source] = temp.addNode();
          temp_node_labels[mapping[source]] = node_labels[source];
        }
        if(mapping[target] == null_node){
          mapping[target] = temp.addNode();
          temp_node_labels[mapping[target]] = node_labels[target];
        }
        ListDigraph::Arc temp_arc = temp.addArc(mapping[source], mapping[target]);
        temp_arc_labels[temp_arc] = arc_labels[a];
        temp_arc_weights[temp_arc] = arc_weights[a];
      }
    }

    if(terminate_loop == false){
      string output_filename = filename + "_decomp_" + to_string(index);
      DigraphWriter<ListDigraph>(temp, output_filename)
        .nodeMap("label", temp_node_labels)
        .arcMap("label", temp_arc_labels)
        .arcMap("weight", temp_arc_weights)
        .run();
    }

    index++;
  }

  return 0;
}