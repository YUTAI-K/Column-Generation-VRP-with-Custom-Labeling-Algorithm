#ifndef _GRAPH_H
#define _GRAPH_H

#include <fstream>
#include <cstdlib>
#include <map>
#include <ranges>
#include <utility>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <cmath>


struct Vertex {
    bool departing_depot;
    bool returning_depot;
    double demand;
};

struct Arc {
    size_t id;
    double cost;
    double original_cost;
};

struct Instance {
    double capacity;
};

// adjacency_list<OutEdgeList, VertexList, Directed,
//                VertexProperties, EdgeProperties,
//                GraphProperties, EdgeList>

using BoostGraph = boost::adjacency_list<
        boost::vecS, // OutEdgeList
        boost::vecS, // VertexList
        boost::bidirectionalS, // Directed
        Vertex, // VertexProperties(DEPARTING, RETRUNING, DEMAND)
        Arc, // EdgeProperties(ID, COST, ORIGINAL COST)
        Instance, // GraphProperties(MAX CAPACITY CONSTRAINT)
        boost::vecS>; // EdgeList
using BoostVertex = BoostGraph::vertex_descriptor;
using BoostArc = BoostGraph::edge_descriptor;

struct Graph {
    BoostGraph g;
    std::map<size_t, BoostVertex> vertices; // An ordered map of indexes to vertex_descriptor's
    std::map<std::pair<size_t, size_t>, BoostArc> arcs; // An ordered map of pairs of indexes to edge_descriptor's

    // Method returns number of vertices
    auto n_vertices() const {
        return boost::num_vertices(g);
    }

    // Method returns number of custumors
    auto n_customers() const {
        return n_vertices() - 2u;
    }

    auto departing_depot() const {
        return vertices.at(0u);
    }

    auto returning_depot() const {
        return vertices.at(n_vertices() - 1u);
    }

    auto customers() const {
        return vertices
            |   std::views::filter([&] (auto i) {
                    const auto& [key, val] = i;
                    return val != departing_depot() and val != returning_depot();
                })
            |   std::views::values;
    }

    auto capacity() const {
        return g[boost::graph_bundle].capacity;
    }

    auto demand(size_t i) const {
        return g[vertices.at(i)].demand;
    }

    auto cost(size_t i, size_t j) const {
        return g[arcs.at({i, j})].cost;
    }
};

std::tuple<Graph, std::vector<double>, std::vector<double>> generate_random_graph(size_t n_customers, unsigned int seed, double capacity);
void write_graphviz(const Graph& g, const std::string& filename);
void print_grid(const Graph& g, const std::vector<double>& xs, const std::vector<double>& ys);
void print_graph(const Graph& g, const std::vector<double>& xs, const std::vector<double>& ys);


#endif