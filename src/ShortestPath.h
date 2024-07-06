#ifndef SHORTEST_PATH_H // I change it from _SHORTEST_PATH_H to this one
#define SHORTEST_PATH_H

#include "Graph.h"
#include "ElementaryLabel.h"
#include "Route.h"
#include <map>
#include <ranges>

struct ShortestPathSolver {
    Graph& g; // Object of type boost::adjacency_list to store the graph

    // Constructor
    explicit ShortestPathSolver(Graph& g) : g{g} {} // explicit: This keyword is used before the constructor to prevent the compiler from using that constructor for implicit type conversions.

    // Method to initialize the labels by solving a relaxed version of the problem
    std::vector<Route> solve_ignoring_shortest_path(const std::map<BoostVertex, double>& duals);

    // Methods to solve the sub problem(second stage)
    std::vector<Route> solve_shortest_path(const std::map<BoostVertex, double>& duals);
    // Function to inplement the incremental shortest_path method
    std::vector<Route> solve_incremental_shortest_path(const std::map<BoostVertex, double>& duals);
    //
    std::vector<Route> solve_incremental_k_best_shortest_path(const std::map<BoostVertex, double>& duals, int k);
    // Function to implement the ignoring-incremental method
    std::vector<Route> solve_ignoring_incremental_shortest_path(const std::map<BoostVertex, double>& duals);
    // Function to implement the ignoreing-incremental method with the concept of multiplicity.
    std::vector<Route> solve_ignoring_incremental_multiplicity_shortest_path(const std::map<BoostVertex, double>& duals);

private:
    std::vector<Route> routes_from_paths( // Method to create the vector of new routes(Route objects) found by elementary labeling
            const std::vector<std::vector<BoostArc>>& edges,
            const std::vector<ElementaryLabel>& labels) const;

    std::vector<Route> routes_from_paths_only_biggest(
        std::vector<std::vector<BoostArc>>& edges, // Matrix of arcs
        std::vector<ElementaryLabel>& labels,
        int k) const; // // Vector of labels

    std::set<size_t> check_negative_loops(
        const std::vector<std::vector<BoostArc>>& edges, // Matrix of arcs
        const std::vector<ElementaryLabel>& labels) const; // // Vector of labels

    std::set<size_t> check_negative_loops_and_multiplicity(
    const std::vector<std::vector<BoostArc>>& edges, // Matrix of arcs
    const std::vector<ElementaryLabel>& labels) const; // // Vector of labels

    // std::set<size_t> check_negative_loops_only_smallest(
    //     std::vector<std::vector<BoostArc>>& edges, // Matrix of arcs
    //     std::vector<ElementaryLabel>& labels); // // Vector of labels

    std::set<size_t> check_negative_loops_in_one_path(
        const std::vector<BoostArc>& edges,
        const ElementaryLabel& label) const;

    std::pair<std::vector<std::vector<BoostArc>>, std::vector<ElementaryLabel>>
        sort_routes_by_cost(
            std::vector<std::vector<BoostArc>>& edges,
            std::vector<ElementaryLabel>& labels) const;

    bool check_existence_of_loops(
    const std::vector<BoostArc>& edges,
    const ElementaryLabel& label) const; // // Vector of labels
};

#endif
