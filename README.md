# Column-Generation-VRP-with-Custom-Labeling-Algorithm

This project code is written and maintained by me, however, I would like to thank my supervisor, [Prof. Alberto Santini](https://santini.in/), for his help.

If you have any questions or find any issues, please contact me at [yutai.ke@bse.eu](mailto:yutai.ke@bse.eu).

## Requirements
- C++ version 23 or later
- Valid Gurobi library and license
- Boost Graph Library (Recommend using vcpkg to manage the dependencies)
- Preferred OS: Windows and Linux are tested. For MacOS small adjustment to Cmake files might be necessary.

## Usage
1. Clone this repository:
    ```sh
    git clone https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm.git
    ```
2. Build the project.
3. For a detailed explanation of the methods and algorithms used, please consult the [User Manual](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/User_Manual.pdf).
4. If you want to run it directly please look at the folder ["Prebuilt application(exe) and logs"](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/tree/main/Prebuilt%20application(exe)%20and%20logs), it contains the console application built from the project code and logs of running the application selecting different algorithms. 

## Running the applications
- Prompts the user to input the number of customers to serve.
- Generates a random graph including customers and the starting & ending depot. Provides visualization. 
- Asks the user to specify the capacity limit for each vehicle.
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

## Algorithm Testing Report
Please consult the [Algorithm Testing Report](https://github.com/YUTAI-K/Column-Generation-VRP-with-Custom-Labeling-Algorithm/blob/main/Algorithm_testing_report.pdf) for detailed testing results and analysis.
