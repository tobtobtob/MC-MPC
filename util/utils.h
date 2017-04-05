#ifndef UTILS_H_
#define UTILS_H_

#include <lemon/list_graph.h>

using namespace lemon;

ListDigraph::Node add_source(ListDigraph& g);

ListDigraph::Node add_sink(ListDigraph& g);

#endif /* UTILS_H_ */