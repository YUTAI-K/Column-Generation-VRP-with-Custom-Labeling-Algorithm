cmake_minimum_required(VERSION 3.23) # Require at least CMake version 3.16
project(cg) # Declare the project named 'cg'

option(CXX "enable C++ compilation" ON) # Option to enable C++ compilation, default is ON

# Set the C++ standard to C++23, required for zip
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CXX)
    enable_language(CXX) # Enable C++ language if the CXX option is ON
endif()

# Add the current directory to CMake's module path to find FindGUROBI.cmake
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# If cmake build type not set then set it to release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}") # Display the build type

# If using Visual Studio compiler with static runtime libraries
if(MSVC AND MT)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT") # Set release flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd") # Set debug flags
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT") # Set release flags for C
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd") # Set debug flags for C
endif()

# Add executable named 'cg' with source files
add_executable(cg
        src/Graph.cpp
        src/Graph.h
        src/main.cpp
        src/Route.cpp
        src/Route.h
        src/Solver.cpp
        src/Solver.h
        src/ShortestPath.h
        src/ShortestPath.cpp
        src/ElementaryLabel.cpp
        src/ElementaryLabel.h
        src/Solver_2.cpp
        src/Solver_3.cpp
)

# Set compiler-specific options
if(MSVC)
    set(WARNING_OPTIONS     /W4) # High warning level
    set(DEBUG_OPTIONS       /Od /Zi) # Debug options
    set(GDB_DEBUG_OPTIONS   /Zi /DEBUG) # GDB debug options
    set(LINKER_OPTIONS      /INCREMENTAL:NO /OPT:REF) # Linker options
else()
    set(WARNING_OPTIONS     -Wall -Wextra -Werror) # High warning level for GCC
    set(GCC_WARNING_OPTIONS -Wduplicated-cond -Wlogical-op -Wuseless-cast -Wrestrict) # Additional GCC warnings
    set(DEBUG_OPTIONS       -O0 -g) # Debug options
    set(GDB_DEBUG_OPTIONS   -ggdb) # GDB debug options
    set(LINKER_OPTIONS      -flto) # Linker options
endif()

# Find required packages
find_package(GUROBI REQUIRED) # Find GUROBI package
find_package(Boost REQUIRED COMPONENTS graph) # Find Boost graph component

# Link boost headers
target_include_directories(cg PRIVATE ${Boost_INCLUDE_DIRS})

# Link Gurobi headers
target_include_directories(cg PRIVATE ${GUROBI_INCLUDE_DIRS})

# Link gurobi libraries to the target
target_link_libraries(cg optimized ${GUROBI_CXX_LIBRARY}
        debug ${GUROBI_CXX_DEBUG_LIBRARY})
target_link_libraries(cg ${GUROBI_LIBRARY})

# Link boost libraries
target_link_libraries(cg  ${Boost_LIBRARIES})

# Include and summarize features if this is the top-level project
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    include(FeatureSummary)
    feature_summary(WHAT ALL)
endif()

# Display the paths of the Gurobi libraries
message(STATUS "GUROBI_LIBRARY IS ${GUROBI_LIBRARY}")
message(STATUS "GUROBI_CXX_LIBRARY IS ${GUROBI_CXX_LIBRARY}")
message(STATUS "GUROBI_CXX_DEBUG_LIBRARY IS ${GUROBI_CXX_DEBUG_LIBRARY}")
