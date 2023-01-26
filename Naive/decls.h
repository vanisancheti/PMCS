/*
 * File Info: decls.h
 * Description: This file contains the declarations common to all the files
 * */

#ifndef _DECLS_H_
#define _DECLS_H_

#include "../boost/graph/graph_traits.hpp"
#include "../boost/graph/adjacency_list.hpp"

#define MAXLEN 512
#define SUPPORT_THRESHOLD 0.5
#define HighFreq(x, y) (x >= SUPPORT_THRESHOLD*y)?1:0
#define LowFreq(x, y) (x < SUPPORT_THRESHOLD*y)?1:0

typedef int NodeID;
typedef int NodeLabel;
typedef std::pair<std::pair<int, int>, int > HiddenEdge;
typedef int GraphType;

typedef boost::property<boost::vertex_color_t, int> VertexProperty;
typedef boost::property<boost::edge_color_t, int> EdgeProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty, EdgeProperty> BoostGraph;
typedef boost::property_map<BoostGraph, boost::vertex_index_t>::type vertex_index;
typedef boost::property_map<BoostGraph, boost::vertex_color_t>::type vertex_color;
typedef boost::property_map<BoostGraph, boost::edge_color_t>::type edge_color;

typedef boost::graph_traits<BoostGraph>::adjacency_iterator adjacency_iterator;
typedef boost::graph_traits<BoostGraph>::edge_iterator edge_iterator;
typedef boost::graph_traits<BoostGraph>::out_edge_iterator out_edge_iterator;
typedef boost::graph_traits<BoostGraph>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<BoostGraph>::edge_descriptor Edge;
typedef boost::graph_traits<BoostGraph>::vertex_descriptor Node;

typedef std::pair<int, std::map<int, std::pair<std::map<int, int>, std::map<int, int> > > > Embedding;

#define GRAPH_EMPTY 0
#define GRAPH_SINGLE_NODE 1
#define GRAPH_SINGLE_EDGE 2
#define GRAPH_GENERAL 3

#endif
