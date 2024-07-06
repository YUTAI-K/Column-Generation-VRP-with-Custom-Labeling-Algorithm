#include "Route.h"
#include "Graph.h"
#include <algorithm>
#include <iostream>
#include <cassert>
#include <vector>

Route::Route(std::vector<BoostVertex> rt_vertices, const BoostGraph& g) :
    vertices{std::move(rt_vertices)}
{
    assert(vertices.size() > 2u);
    assert(g[vertices.front()].departing_depot);
    assert(g[vertices.back()].returning_depot);

    cost = 0.0;

    for(auto idx = 0u; idx < vertices.size() - 1u; ++idx) { // loop through the vertex_descriptor list to compute the total route cost(sum of individual edge costs)
        const auto& [edge, success] = boost::edge(vertices[idx], vertices[idx + 1u], g);
        if(not success)
        {
            std::cerr << "Edge not found between " << idx << " and " << idx+1 << "\n";
        }

        const auto edge_cost = g[edge].original_cost;

        if(edge_cost < 0.0)
        {
            std::cerr << "Edge between " << idx << " and " << idx+1 << " has a negative cost (" << edge_cost << ") in the instance!\n";
        }

        cost += edge_cost;

        if(cost < 0.0)
        {
            std::cerr << "Arrived at a negative cost while building route: " << cost << "\n";
        }
    }
}

std::ostream &operator<<(std::ostream &out, const Route &r) {
    for(auto v : r.vertices) { // Printing all passing vertexes
        out << v << " ";
    }

    out << "(" << r.cost << ")"; // Print the cost

    return out;
}
