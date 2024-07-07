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
    auto [g, xs, ys]  = generate_random_graph(24u, rd()); // Generating random graph
    // Print the grid visualization of the graph
    print_grid(g, xs, ys);

    // Now start to solve the problem
    auto solver = Solver{g};
    solver.solve_ignoring_incremental_multiplicity_cg();

    system("pause"); // Avoid instant quit
    return 0;
}