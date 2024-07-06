#ifndef _SOLVER_H
#define _SOLVER_H

#include "Graph.h"
#include "Route.h"
#include "ShortestPath.h"
#include "gurobi_c++.h"
#include <memory>
#include <vector>

struct Solver {
    Graph& g; // A graph(adjacency_list)
    ShortestPathSolver sp_solver; // A shortest path solver(second stage)

    std::vector<Route> routes; // Consult Route.h and Route.cpp

    GRBEnv e;
    std::unique_ptr<GRBModel> m; // Pointer necessary here

    std::vector<GRBVar> x; // Variables(columns)
    std::map<BoostVertex, GRBConstr> covering; // A map of boost::vertex_descriptor to gurobi constraints. This is the covering constraints

    Solver(Graph& g); // Generator

    void solve_cg(); // Basic method
    void solve_incremental_cg();// With incremental resouce method inmplemented
    void solve_incremental_k_best_cg(int k); // Same as before but only returns the k best routes
    void solve_ignoring_incremental_cg(); // Solve a ignoring incremental algorithm
    void solve_ignoring_incremental_multiplicity_cg(); // Solve with the concept of multiplicity


private:
    void add_route(const Route& route);
    void add_route(const std::vector<BoostVertex>& vertices);
};

#endif