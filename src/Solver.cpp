#include "Solver.h"
#include "ShortestPath.h"
#include "gurobi_c++.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

// This CPP file includes help funcitons and the method to solve a normal shortest path(algorithm 1)

//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////The Constructor///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
Solver::Solver(Graph& g) : // Constructor
    g{g}, sp_solver{g}, e{true}
{
    ////////////////////////////////House keeping/////////////////////////////////////////////
    // Start the environment manually.
    // Useful if you want to suppress Gurobi's output.
    try {
        e.set(GRB_IntParam_OutputFlag, 0);
        e.set(GRB_IntParam_LogToConsole, 0);
        e.start();
    } catch(GRBException& e) {
        std::cerr << "Cannot start Gurobi: " << e.getMessage() << "\n";
        throw;
    }

    //////////////////////////////Creating Gurobi Model and Pointer/////////////////////////////////////
    /*std::make_unique is a utility function in C++ that was introduced in C++14.
    It is used to create a unique_ptr object,
    which is a smart pointer that manages the lifetime of dynamically allocated objects.
    It is defined inside <memory> header file.*/
    // GRBModel m{e};
    m = std::make_unique<GRBModel>(e); // So m is a pointer pointing to the model

    ////////////////////////////////////Setting up Constraints///////////////////////////////////////////////
    // # of constraints = # of customers to visit
    // A stringstream is used to dynamically build a string. It’s a stream class to operate on strings
    std::stringstream ss;
    for(auto v : g.customers()) { // Retunrs the custumor verteces
        ss.str(std::string{}); // This resets the content of the stringstream. It effectively clears any previous content, preparing it for a new string construction.
        ss << "cover_" << v; // ss << "cover_" << v;: This line appends the prefix "cover_" followed by a customer vertex identifier v to the stringstream.
        covering[v] = m->addConstr(GRBLinExpr{}, // A linear expression consists of a constant term, plus a list of coefficient-variable pairs that capture the linear terms.
            GRB_GREATER_EQUAL,
            1.0, // All customors are visited once
            ss.str());
    }

    ///////////////////////////////////////Initialzing Columns(variables) pool////////////////////////////////////
    for(auto v : g.customers()) { // Initializing the routes pool as Source->v->Sink
        add_route({g.departing_depot(), v, g.returning_depot()}); // For add_route see next function.
    }
}

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////Two functions to add routes//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void Solver::add_route(const std::vector<BoostVertex>& vertices) { // This one takes as input a vector of vertex_descriptor
    add_route(Route{vertices, g.g}); // Calling its own overload function, which is the one bellow.
}

void Solver::add_route(const Route& r) { // This one takes as input a Route object
    // Gurobi column object: A column consists of a list of coefficient, constraint pairs.
    // Columns are used to represent the set of constraints in which a variable participates, and the associated coefficients.
    GRBColumn gcol; // Gurobi column object

    for(auto v : r.vertices) {
        if(v != g.departing_depot() and v != g.returning_depot()) {
            gcol.addTerm(1.0, covering[v]); // Coefficient is 1 and add this term to the corresponding covering constraint
        }
    }

    // x is a vector of gurobi variable objects
    // Gurobi variable object: Variables are always associated with a particular model.
    // You create a variable object by adding a variable to a model (using GRBModel.addVar),
    // rather than by using a GRBVar constructor.
    x.push_back(m->addVar(
            0.0, // Lower bound
            GRB_INFINITY, // Upper bound
            r.cost, // Objective coeff
            GRB_CONTINUOUS, // Variable type
            gcol // Column object
    ));

    routes.push_back(r); // Add the route to the vecetor of routes
}



//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////// The method to solve the whole problem ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

void Solver::solve_cg() {

    // Get the start time
    auto start = std::chrono::high_resolution_clock::now();

    m->optimize(); // Solving the gurobi model(first stage)

    // Check optimatility
    if(m->get(GRB_IntAttr_Status) != GRB_OPTIMAL) { // If not optimal
        std::cerr << "Initial LP is infeasible or unbounded!\n";
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Solved MP. Cost: " << m->get(GRB_DoubleAttr_ObjVal) << "\n"; // If optimal, print the result

    size_t iteration = 1u; // Tracking iterations
    std::vector<Route> new_routes; // Vector to store routes found by shortest path solver
    std::map<size_t, double> duals; // Map to store duals for the constraints

    // //////////////////////////////////////////////////////////////////////////////
    //  // Advanced initialization
    //  for(auto& [v, cst] : covering) {duals[v] = cst.get(GRB_DoubleAttr_Pi);} // Set duals
    //  new_routes = sp_solver.solve_ignoring_shortest_path(duals); // Solve the second stage, generating new columns
    //  for(const auto& route : new_routes) { add_route(route); } // Adding new routes to the model
    //  std::cout << "Initial column pool:\n"; // Printing the column pool
    //  for(const auto& route : routes) { std::cout << "\t" << route << "\n"; }
    //  try { m->optimize(); } catch(const GRBException& e) // Try to resolve the model
    //      { std::cerr << "Cannot solve LP with Gurobi: " << e.getMessage() << "\n"; throw;}
    //  if(m->get(GRB_IntAttr_Status) != GRB_OPTIMAL) { // If not optimal
    //      std::cerr << "Initial LP is infeasible or unbounded!\n";
    //      std::exit(EXIT_FAILURE);}
    //  std::cout << "Solved MP. Cost: " << m->get(GRB_DoubleAttr_ObjVal) << "\n"; // If optimal, print the result
    // /////////////////////////////////////////////////////////////////////////////////


    while(true) {
        std::cout << "\n === Iteration " << iteration << " ===\n"; // Print iteration

        // duals.clear();// Map to record dual multipliers for each constraint

        for(auto& [v, cst] : covering) { // Loop to populate the duals map
            duals[v] = cst.get(GRB_DoubleAttr_Pi);
        }

        new_routes = sp_solver.solve_shortest_path(duals);


        if(new_routes.empty()) { // If still no new columns found, break
            std::cout << "Solved SP. Found no column with <0 reduced cost.\n";
            break;
        }

        std::cout << "Solved SP. Found " << new_routes.size() << " columns with <0 reduced cost.\n"; // Print the number of found columns

        for(const auto& route : new_routes) {
            std::cout << "\t" << route << "\n"; // Print the found columns
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

    // Calculate the duration in the default unit (nanoseconds here)
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    double runtime = duration.count()/1000.0;
    std::cout << "\n\n\n=======================================================\n"
                    << "==================== Problem solved ===================\n"
                    << "=======================================================\n";
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





void Solver::solve_incremental_cg()
{

    // Get the start time
    auto start = std::chrono::high_resolution_clock::now();

    m->optimize(); // Solving the gurobi model(first stage)

    // Check optimatility
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

        // duals.clear();// Map to record dual multipliers for each constraint

        for(auto& [v, cst] : covering) { // Loop to populate the duals map
            duals[v] = cst.get(GRB_DoubleAttr_Pi);
        }


        new_routes = sp_solver.solve_incremental_shortest_path(duals);


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

    // Calculate the duration in the default unit (nanoseconds here)
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    double runtime = duration.count()/1000.0;

    std::cout << "\n\n\n=======================================================\n"
                << "==================== Problem solved ===================\n"
                << "=======================================================\n";
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






void Solver::solve_incremental_k_best_cg(int k)
{

    // Get the start time
    auto start = std::chrono::high_resolution_clock::now();

    m->optimize(); // Solving the gurobi model(first stage)

    // Check optimatility
    if(m->get(GRB_IntAttr_Status) != GRB_OPTIMAL) { // If not optimal
        std::cerr << "Initial LP is infeasible or unbounded!\n";
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Solved MP. Cost: " << m->get(GRB_DoubleAttr_ObjVal) << "\n"; // If optimal, print the result

    size_t iteration = 1u; // Tracking iterations
    std::vector<Route> new_routes; // Vector to store routes found by shortest path solver
    std::map<size_t, double> duals; // Map to store duals for the constraints

    while(true) {
        auto start_iter_time = std::chrono::high_resolution_clock::now();
        std::cout << "\n === Iteration " << iteration << " ===\n"; // Print iteration

        // duals.clear();// Map to record dual multipliers for each constraint

        for(auto& [v, cst] : covering) { // Loop to populate the duals map
            duals[v] = cst.get(GRB_DoubleAttr_Pi);
        }

        new_routes = sp_solver.solve_incremental_k_best_shortest_path(duals, k);

        // Get the end time
        auto end_iter_time = std::chrono::high_resolution_clock::now();

        // Calculate the duration in the default unit (milliseconds here)
        auto duration_iter = std::chrono::duration_cast<std::chrono::milliseconds>(end_iter_time - start_iter_time);
        double runtime_iter = duration_iter.count()/1000.0;
        std::cout<< "run time for iteration " << iteration << " is "<< runtime_iter << '\n';


        if(new_routes.empty()) { // If still no new columns found, break
            std::cout << "Solved SP. Found no column with <0 reduced cost.\n";
            break;
        }

        std::cout << "Solved SP. Found " << new_routes.size() << " columns with <0 reduced cost.\n"; // Print the number of found columns

        for(const auto& route : new_routes) {
            std::cout << "\t" << route << "\n"; // Print the found columns
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

    std::cout << "\n\n\n=======================================================\n"
                << "==================== Problem solved ===================\n"
                << "=======================================================\n";

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