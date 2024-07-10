#include "Graph.h"
#include "Solver.h"
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>
#include <tuple>

int main() {

    std::cout << "Developer: Yutai Ke." << " For more details consult the readme file and the user manual in the reporsitory. " << '\n'
              << "Special thanks to Prof. Alberto Santini" << '\n'
              << "=======================================================\n\n";

    std::random_device rd; // initializing device
    int number_of_customers;
    int solver_choice;
    double capacity;

    // Prompt user for number of customers
    std::cout << "Enter the number of customers (int, better below 20 if you want a quick solution): ";
    while (!(std::cin >> number_of_customers) || number_of_customers <= 0) {
        std::cout << "Invalid input. Please enter a positive integer: ";
        std::cin.clear(); // Clear the error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
    }

    // Prompt user for capacity
    std::cout << "Enter the capacity: \n";
    std::cout << "Input 0 to use the default method.\n";
    std::cout << "You can also input a number between 0 and 1 (boundaries not included) to set the capacity to that proportion of the total capacity demand of all customers.\n";
    std::cout << "Or input a number more than 1 to set the capacity limit to that number.\n";
    std::cout << "In all cases, if the number you inputted ends up being smaller than max{All customers' demand}, it will automatically be scaled up to that number.\n";
    while (!(std::cin >> capacity) || capacity < 0) {
        std::cout << "Invalid input. Please enter a valid number (0 or a positive number): ";
        std::cin.clear(); // Clear the error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
    }

    // Generate random graph
    auto [g, xs, ys]  = generate_random_graph(number_of_customers, rd(), capacity); // Generating random graph

    std::cout << "\n============================================================" << '\n';
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

    std::cout << "\n\n============================================"
                   <<"Start solving the problem\n"
                << "SP stands for subproblem, MP stands for master problem.\n\n";
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