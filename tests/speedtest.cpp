#include <lemon/list_graph.h>
#include "../decomposition.h"
#include "../MPC.h"
#include <stdlib.h>
#include <time.h>
#include <lemon/bfs.h>
#include <lemon/cost_scaling.h>
#include <lemon/lgf_reader.h>
#include "../MPC_IBFS.h"

using namespace std;
using namespace lemon;

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


void speedtest1(int k, int n, int m)
{
  
  srand(time(NULL));
  ListDigraph g;
  ListDigraph::ArcMap<int> demands(g);
  ListDigraph::ArcMap<int> weights(g);
  createKPathGraph(g, k,n,m, weights, demands);
    
  ListDigraph::Node s, t;
  
  s = addSource(g);
  t = addSink(g);

  //MINFLOW

  clock_t begin_time = clock();

  CostScaling<ListDigraph> costScaling(g);

  costScaling.lowerMap(demands);
  costScaling.costMap(weights);

  ListDigraph::Arc extraArc = g.addArc(s, t);
  costScaling.stSupply(s, t, 1000);
  bool result = costScaling.run();

  clock_t after_minflow = clock();

  g.erase(extraArc);

  //DECOMPOSITION

  ListDigraph::ArcMap<int> minflow(g);

  find_minflow(g, demands, minflow, s, t);

  clock_t after_maxflow = clock();

  int num_ants = 0;
  for (ListDigraph::OutArcIt o(g, s); o != INVALID; ++o)
  {
    num_ants += minflow[o];
  }
  vector<ListDigraph::Node*> decomposition;

  decompose_graph(g, minflow, s, t, decomposition);

  clock_t after_decomposition = clock();
  
  cout  << "\033[0;32m" << "SPEEDTEST 1: decomposition with lemon maxflow\n";
  cout << "\033[0;0m" << "Time required for minflow computation: " << (after_minflow - begin_time) << "\n";
  cout << "Time required for decomposition: " << (after_maxflow - after_minflow) << " (flow) + " << (after_decomposition - after_maxflow) << " (decomposition) \n\n";

}



void speedtest2(int k, int n, int m)
{
    
  ListDigraph g;
  ListDigraph::ArcMap<int> demands(g);
  ListDigraph::ArcMap<int> weights(g);
  createKPathGraph(g, k,n,m, weights, demands);
    
  ListDigraph::Node s, t;
  
  s = addSource(g);
  t = addSink(g);

  //MINFLOW

  clock_t begin_time = clock();
  ListDigraph::Arc extraArc = g.addArc(s, t);
  CostScaling<ListDigraph> costScaling(g);

  costScaling.lowerMap(demands);
  costScaling.costMap(weights);
  costScaling.stSupply(s, t, 1000);
  costScaling.run();

  ListDigraph::ArcMap<int> minmin(g);
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    minmin[a] = costScaling.flow(a);
  }
  

  clock_t after_minflow = clock();

  //DECOMPOSITION

  ListDigraph::ArcMap<int> dummyCosts(g);
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    dummyCosts[a] = 1;
  }
  dummyCosts[extraArc] = 0;

  CostScaling<ListDigraph> costScaling2(g);

  costScaling2.lowerMap(demands);
  costScaling2.costMap(dummyCosts);

  costScaling2.stSupply(s, t, 1000);
  costScaling2.run();



  ListDigraph::ArcMap<int> minflow(g);
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    minflow[a] = costScaling2.flow(a);
  }
  //drawGraphToFileWithArcMap(g, minflow);
  g.erase(extraArc);
  //find_minflow(g, demands, minflow, s, t);


  clock_t after_maxflow = clock();

  vector<ListDigraph::Node*> decomposition;

  decompose_graph(g, minflow, s, t, decomposition);

  clock_t after_decomposition = clock();
  
  cout  << "\033[0;32m" << "SPEEDTEST 2: decomposition with unweighted lemon minflow\n";
  cout<< "\033[0;0m" << "Time required for minflow computation: " << (after_minflow - begin_time) << "\n";
  cout << "Time required for decomposition: " << (after_maxflow - after_minflow) << " (flow) + " << (after_decomposition - after_maxflow) << " (decomposition) \n\n";

}


void speedtest5(int k, int n, int m)
{
    
  ListDigraph g;
  ListDigraph::ArcMap<int> demands(g);
  ListDigraph::ArcMap<int> weights(g);
  createKPathGraph(g, k,n,m, weights, demands);
    
  ListDigraph::Node s, t;
  
  s = addSource(g);
  t = addSink(g);

  //MINFLOW

  clock_t begin_time = clock();
  ListDigraph::Arc extraArc = g.addArc(s, t);
  CostScaling<ListDigraph> costScaling(g);

  costScaling.lowerMap(demands);
  costScaling.costMap(weights);
  costScaling.stSupply(s, t, 1000);
  costScaling.run();

  ListDigraph::ArcMap<int> minmin(g);
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    minmin[a] = costScaling.flow(a);
  }
  
  g.erase(extraArc);
  clock_t after_minflow = clock();

  //DECOMPOSITION

  ListDigraph::NodeMap<int> labels(g);
  int label_counter = 0;
  for (ListDigraph::NodeIt n(g); n != INVALID; ++n)
  {
    labels[n] = label_counter;
    label_counter++;
  }

  ListDigraph::ArcMap<int> flow(g);
  find_minflow_IBFS(g, demands, flow,labels,  s, t);



  clock_t after_maxflow = clock();

  vector<ListDigraph::Node*> decomposition;

  //decompose_graph(g, flow, s, t, decomposition);

  clock_t after_decomposition = clock();
  
  cout << "\033[0;32m" << "SPEEDTEST 5: lemon minflow against IBFS maxflow\n";
  cout << "\033[0;0m" << "Time required for minflow computation: " << (after_minflow - begin_time) << "\n";
  cout << "Time required for decomposition: " << (after_maxflow - after_minflow) << " (flow) + " << (after_decomposition - after_maxflow) << " (decomposition) \n\n";

}

void speedtest6(int k, int n, int m)
{
    
  ListDigraph g;
  ListDigraph::ArcMap<int> demands(g);
  ListDigraph::ArcMap<int> weights(g);
  createKPathGraph(g, k,n,m, weights, demands);
    
  ListDigraph::Node s, t;
  
  s = addSource(g);
  t = addSink(g);

  //MINFLOW

  clock_t begin_time = clock();
  ListDigraph::Arc extraArc = g.addArc(s, t);
  CostScaling<ListDigraph> costScaling(g);

  costScaling.lowerMap(demands);
  costScaling.costMap(weights);
  costScaling.stSupply(s, t, 1000);
  costScaling.run();

  ListDigraph::ArcMap<int> minmin(g);
  for(ListDigraph::ArcIt a(g); a != INVALID; ++a){
    minmin[a] = costScaling.flow(a);
  }
  
  g.erase(extraArc);
  clock_t after_minflow = clock();

  //DECOMPOSITION
  
  ListDigraph::ArcMap<int> flow(g);
  find_minflow_new(g, flow, s, t);

  //drawGraphToFileWithArcMap(g, flow)



  clock_t after_maxflow = clock();

  vector<ListDigraph::Node*> decomposition;

  decompose_graph(g, flow, s, t, decomposition);

  clock_t after_decomposition = clock();
  
  cout << "\033[0;32m" << "SPEEDTEST 6: lemon minflow against new minflow\n";
  cout << "\033[0;0m" << "Time required for minflow computation: " << (after_minflow - begin_time) << "\n";
  cout << "Time required for decomposition: " << (after_maxflow - after_minflow) << " (flow) + " << (after_decomposition - after_maxflow) << " (decomposition) \n\n";

}


/**

void speedtest3(int k, int n, int m)
{
    
  ListDigraph g;
  ListDigraph::ArcMap<int> demands(g);
  
  ListDigraph::Node a = g.addNode();
  ListDigraph::Node b = g.addNode();
  ListDigraph::Node c = g.addNode();
  ListDigraph::Node d = g.addNode();

  ListDigraph::Arc ab = g.addArc(a, b);
  ListDigraph::Arc cd = g.addArc(c, d);

  demands[ab] = 1;
  demands[cd] = 1;

  //DECOMPOSITION

  ListDigraph::NodeMap<int> labels(g);
  int label_counter = 0;
  for (ListDigraph::NodeIt n(g); n != INVALID; ++n)
  {
    labels[n] = label_counter;
    label_counter++;
  }

  ListDigraph::ArcMap<int> flow(g);

  ListDigraph::Node s = addSource(g);
  ListDigraph::Node t = addSink(g);

  find_minflow_IBFS(g, demands, flow, labels,s, t);

  vector<ListDigraph::Node*> decomposition;

  decompose_graph(g, flow, s, t, decomposition);

}**/

int main(){

  int k = 10;
  int n = 10000;
  int m = 10000;

  //speedtest1(k,n,m);
  speedtest2(k,n,m);
  //speedtest3(k,n,m);
  //speedtest4(k,n,m);
  speedtest5(k,n,m);
  speedtest6(k,n,m);

  return 0;
}