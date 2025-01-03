# Column-Generation-VRP-with-Custom-Labeling-Algorithm
This repository is part of my master’s thesis, **"Enhancing Algorithm Efficiency for the Vehicle Routing Problem: A Heuristic Approach Based on Column Generation"**. The project focuses on improving algorithmic efficiency for solving the Vehicle Routing Problem (VRP) through a novel heuristic approach, utilizing column generation combined with a custom labeling algorithm.

The original research paper is available on this repository, click [here to access it.](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/Enhancing_Algorithm_Efficiency_for_the_Vehicle_Routing_Problem__A_Heuristic_Approach_Based_on_Column_Generation_.pdf).

This project code is written and maintained by me, however, I would like to thank my supervisor, [Prof. Alberto Santini](https://santini.in/), for his help.

If you have any questions or find any issues, please contact me at [yutai.ke@bse.eu](mailto:yutai.ke@bse.eu).

## Requirements
- C++ version 23 or later
- Valid Gurobi library and license
- Boost Graph Library 

## Usage
1. Clone this repository:
    ```sh
    git clone https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm.git
    ```
2. Build the project. This project requires the boost graph library, which can be installed by following the instructions on its [official website](https://www.boost.org/), or by using a package manager like apt for linux, Homebrew for macOS, or vcpkg for Windows.
3. For a detailed explanation of the methods and algorithms used, please consult the [paper](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/Enhancing_Algorithm_Efficiency_for_the_Vehicle_Routing_Problem__A_Heuristic_Approach_Based_on_Column_Generation_.pdf).


## Running the application
When executed, the console applciation will:
- Prompts the user to input the number of customers to serve.
- Prompts the user to input an integer for seeding, which guarantees reproducibility.
- Asks the user to specify the capacity limit for the vehicles, instructions are provided.
- Generates a random graph including customers and the starting & ending depot. Provides visualization. 
- Offers a choice among five algorithms, some of which I developed myself.
- Solves the problem and reports the results.

## Project Structure
The project source code is organized as follows:

- **[src](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/tree/main/src)**: Contains the source code.
  - **[main.cpp](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/src/main.cpp)**: Contains the main function of the project.
  - **[Graph.h](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/src/Graph.h)**: Defines basic graph structures, including functions to generate random graph instances and visualization/preprocessing functions.
  - **[Route.h](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/src/Route.h)**: Contains route-related data structures and functions.
  - **[Solver.h](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/src/Solver.h)**: Defines the "Solver" structure used to solve VRP problems with different algorithms. Each algorithm is associated with a different method in this structure.
  - **[ShortestPath.h](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/src/ShortestPath.h)**: Defines the "ShortestPathSolver" structure, a part of the "Solver" structure, used to solve the subproblem of the column generation algorithm, which is a resource-constrained shortest path problem with self-defined labeling algorithms. Each algorithm is associated with a unique method.
  - **[ElementaryLabel.h](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/src/ElementaryLabel.h)**: Defines the structures and utils used in the "ShortestPathSolver" structure, including labels and resources.

- **[CMakeLists.txt](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/CMakeLists.txt) & [CMakeSettings.json](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/CMakeSettings.json)**: The CMake files necessary
for configuring the build conditions for the project.
- **[FindGurobi.cmake](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/FindGurobi.cmake)**: The CMake file necessary to find Gurobi on your computer, please visit the [Gurobi Support Website](https://support.gurobi.com/hc/en-us/articles/360039499751-How-do-I-use-CMake-to-build-Gurobi-C-C-projects) for details.
- **[vcpkg.json](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/vcpkg.json)**: The vcpkg file used to manage dependencies. 

