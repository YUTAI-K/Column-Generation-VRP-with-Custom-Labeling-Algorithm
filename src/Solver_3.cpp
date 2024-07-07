#pragma once

#include "Solver.h"
#include "ShortestPath.h"
#include "gurobi_c++.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

void Solver::solve_ignoring_incremental_multiplicity_cg() // ALgorithm 5
{
    // Get the start time
    auto start = std::chrono::high_resolution_clock::now();

    m->optimize(); // Solving the gurobi model(first stage)

    // Check optimatility(should always be solvable)
    if(m->get(GRB_IntAttr_Status) != GRB_OPTIMAL) { // If not optimal
        std::cerr << "Initial LP is infeasible or unbounded!\n";
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Solved MP. Cost: " << m->get(GRB_DoubleAttr_ObjVal) << "\n"; // If optimal, print the result

    size_t iteration = 1u; // Tracking iterations
    std::vector<Route> new_routes; // Vector to store routes found by shortest path solver
    std::map<size_t, double> duals; // Map to store duals for the constraints

    while(true) {
        std::cout << "\n === Iteration " << iteration << " ===\n"; // Print iteration

        for(auto& [v, cst] : covering) // Loop to populate the duals map
        {
            duals[v] = cst.get(GRB_DoubleAttr_Pi);
        }

        new_routes = sp_solver.solve_ignoring_shortest_path(duals); // Solve the second stage, generating new columns

        if(new_routes.empty()) { // If no new columns found, solve again with the incremental shortest path
            std::cout << "\t=== Ignoring algorithm fails to find any new routes, adding resource(s) with biggest multiplicity ===";
            new_routes = sp_solver.solve_ignoring_incremental_multiplicity_shortest_path(duals);
        }

        if(new_routes.empty()) { // If still no new columns found, break
            std::cout << "Solved SP. Found no column with <0 reduced cost.\n";
            break;
        }

        std::cout << "Solved SP. Found " << new_routes.size() << " columns with <0 reduced cost.\n"; // Print the number of found columns

        for(const auto& route : new_routes) {
            // std::cout << "\t" << route << "\n"; // Print the found columns
            add_route(route); // Add this to our model
        }

        try { // Try to solve the model again
            m->optimize();
        } catch(const GRBException& e) {
            std::cerr << "Cannot solve LP with Gurobi: " << e.getMessage() << "\n";
            throw;
        }

        if(m->get(GRB_IntAttr_Status) != GRB_OPTIMAL) {
            std::cerr << "LP is infeasible or unbounded at iteration " << iteration << "!\n";

            std::stringstream ss;
            ss << "infeasible_lp_iter_" << iteration << ".lp";

            m->write(ss.str());

            std::cerr << "Model file written to " << ss.str() << "\n";

            std::exit(EXIT_FAILURE);
        }

        std::cout << "Solved MP. Cost: " << m->get(GRB_DoubleAttr_ObjVal) << "\n"; // If no bullshit happened print the new solved value

        ++iteration;
    }

    // Get the end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration in the default unit (milliseconds here)
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    double runtime = duration.count()/1000.0;

    // Output the time taken
    std::cout << "Time taken: " << runtime << " seconds\n";


    // visulizing the final result
    auto dual_bound  = m -> getObjective().getValue(); // Record the objective value to compare the difference
    GRBVar* result_variables = m -> getVars(); // You are receiving a reference to the eixsting variables.
    std::cout <<"\n=== Final active routes === \n";
    for(int i = 0; i < routes.size();++i)
    {
        double result_var_value = result_variables[i].get(GRB_DoubleAttr_X); // GRB_DoubleAttr_X is used to get the value of the variable
        if (result_var_value > 0)
            std::cout << "\t" << routes[i] << "(" << result_var_value << ")" << "\n";
    }

    // Comparing the difference between binary solution and continuous relaxation.
    for (int i = 0; i < routes.size(); ++i)
    {
        result_variables[i].set(GRB_CharAttr_VType, GRB_BINARY);
    }
    m -> update();
    m -> optimize();
    auto primal_bound = m -> getObjective().getValue();

    std::cout << "\nPrimal_bound = " << primal_bound << '\n'
        << "Dual_bound = " << dual_bound << '\n'
        << "Gap = " << (primal_bound - dual_bound)/primal_bound << '\n';
    std::cout<< "Columns geenrated: "<< m->get(GRB_IntAttr_NumVars) - g.n_customers() << '\n';
    std::cout<< "Iterations run: " << iteration << '\n';

}
