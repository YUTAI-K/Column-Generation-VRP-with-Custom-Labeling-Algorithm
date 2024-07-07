#include "Graph.h"
#include "Solver.h"
#include <random>

// New thought:
// Implement this: When igonoring algorithm can't find any routes, use incremental resource constraint method to add some resources,
// and apply ignoring algorithm again. If I keep add resources until no repeated routes are produced, that will be too much.

// Implement this:
// Use a map to keep track of number of times a vertex is fiund in repeated routes, only add the vertices that appears the most number of times
// Record propotion of negative edges in each iteration


// ALso report the variance of run time so we know which one is more stable
// Report number of columns generated
// Report propotion of routes that has negative cost
// Report the resuced cost

// Build another map with multiplicity as index and a vector of customers as values
// Is this logic really fine? We are ignoring loops if the cost of routes is non negative

// Consider 8, 12, 16 nodes

// Add improvement plot

// Since we only want routes that have negative cost, why don't change the extending threshhold to 0? Well this might not
// Work well if we have a lot of negative edges in the middle but, who knows.
int main() {
    std::random_device rd; // initializing device
    unsigned int number_of_customers;
    int solver_choice;

    // Prompt user for number of customers
    std::cout << "Enter the number of customers(better bellow 20 if you want a quick solution): ";
    std::cin >> number_of_customers;
    // Generate random graph
    auto [g, xs, ys]  = generate_random_graph(number_of_customers, rd()); // Generating random graph

    std::cout << "Random graph generated, the visulizaiton is shown bellow: \n\n";
    // Print the grid visualization of the graph
    print_grid(g, xs, ys);
    std::cout << "\n\n\n";

    // Prompt user for solver choice
    std::cout << "Select the solving method:\n";
    std::cout << "1: solve_cg\n";
    std::cout << "2: solve_incremental_cg\n";
    std::cout << "3: solve_incremental_k_best_cg (enter k after selection)\n";
    std::cout << "4: solve_ignoring_incremental_cg\n";
    std::cout << "5: solve_ignoring_incremental_multiplicity_cg\n";
    std::cout << "Enter your choice: ";
    std::cin >> solver_choice;

    // Initialize solver
    auto solver = Solver{g};

    // Call the appropriate solver function
    switch(solver_choice) {
    case 1:
        solver.solve_cg();
        break;
    case 2:
        solver.solve_incremental_cg();
        break;
    case 3: {
            int k;
            std::cout << "Enter the value of k: ";
            std::cin >> k;
            solver.solve_incremental_k_best_cg(k);
            break;
    }
    case 4:
        solver.solve_ignoring_incremental_cg();
        break;
    case 5:
        solver.solve_ignoring_incremental_multiplicity_cg();
        break;
    default:
        std::cout << "Invalid choice. Exiting...\n";
        return 1;
    }

    // Avoid instant quit
    std::cout << "Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}