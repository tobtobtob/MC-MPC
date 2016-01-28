/*
 * MPC.h
 *
 *  Created on: Nov 29, 2015
 *      Author: topi
 */

#ifndef MPC_H_
#define MPC_H_

#include <lemon/list_graph.h>

using namespace lemon;

void find_feasible_flow(ListDigraph& g, ListDigraph::ArcMap<int>& demands, ListDigraph::ArcMap<int>& flow);

void find_minflow(ListDigraph& g);



#endif /* MPC_H_ */
