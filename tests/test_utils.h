/*
 * test_utils.h
 *
 *  Created on: Nov 29, 2015
 *      Author: topi
 */

#ifndef TESTS_TEST_UTILS_H_
#define TESTS_TEST_UTILS_H_

#include <lemon/list_graph.h>


using namespace lemon;


void createRandomGraph(ListDigraph& g, ListDigraph::NodeMap<int>& labels, int num_nodes, float edge_prob);

void addSourceAndSink(ListDigraph& g);



#endif /* TESTS_TEST_UTILS_H_ */
