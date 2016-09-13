#include <lemon/list_graph.h>
#include "../MC_MPC_solver.h"
#include <stdlib.h>
#include <time.h>
#include "catch.hpp"

using namespace std;
using namespace lemon;


TEST_CASE("simple test case succeeds"){

    ListDigraph g;
    ListDigraph::Node a = g.addNode();
    ListDigraph::Node b = g.addNode();
    ListDigraph::Node c = g.addNode();
    ListDigraph::Node d = g.addNode();
    ListDigraph::Node e = g.addNode();
    ListDigraph::Node f = g.addNode();
    ListDigraph::Node g = g.addNode();

    ListDigraph::Arc ab = g.addArc(a, b);
    ListDigraph::Arc ac = g.addArc(a, c);
    ListDigraph::Arc bd = g.addArc(b, d);
    ListDigraph::Arc cd = g.addArc(c, d);
    ListDigraph::Arc be = g.addArc(b, e);
    ListDigraph::Arc df = g.addArc(d, f);
    ListDigraph::Arc eg = g.addArc(e, g);
    ListDigraph::Arc fg = g.addArc(f, g);
    ListDigraph::Arc de = g.addArc(d, e);

    ListDigraph::NodeMap<bool> decomp1(g, false);
    ListDigraph::NodeMap<bool> decomp2(g, false);
    ListDigraph::NodeMap<bool> decomp3(g, false);

    decomp1[a] = true;
    decomp1[b] = true;
    decomp1[c] = true;

    decomp2[b] = true;
    decomp2[c] = true;
    decomp2[d] = true;
    decomp2[e] = true;
    decomp2[f] = true;

    decomp3[e] = true;
    decomp3[f] = true;
    decomp3[g] = true;

    vector<ListDigraph::NodeMap<bool> > decomposition;
    decomposition.push_back(decomp1);
    decomposition.push_back(decomp2);
    decomposition.push_back(decomp3);

    ListDigraph::ArcMap<int> weights(g, 1);
    weights[be] = 5;

    ListDigraph::ArcMap<int> solution(g);

    solve_MC_MPC(g, decomposition, weights, solution);

    REQUIRE(solution[be] == 0);
    REQUIRE(solution[ab] == 1);
    REQUIRE(solution[df] == 1);

}