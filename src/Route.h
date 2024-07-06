#ifndef _ROUTE_H
#define _ROUTE_H

#include "Graph.h"
#include <cstdlib>
#include <iostream>
#include <vector>

struct Route {
    std::vector<BoostVertex> vertices; //vector of vertex descriptors
    double cost; // The coefficient associated with this route in the target funciton


    // Two constructors
    // Constructor 1: Direct Initialization
    Route(std::vector<BoostVertex > vertices, double cost) :
        vertices{std::move(vertices)}, cost{cost} {}
    // Resource Transfer: When std::move is used on an object,
    // it allows the object to transfer its internal resources to another object.
    // For instance, when moving a std::vector,
    // the internal pointer to the data in the original vector can be transferred to a new vector.
    // The original vector is left in a valid but unspecified state, which typically means it has no resources left,
    // making it safe to destruct but not necessarily usable without reassignment or reconstruction.


    // Constructor 2: Conversion from size_t to BoostVertex
    // The purpose here appears to be to convert these indices or ids to actual vertex descriptors using the graph g.
    Route(std::vector<size_t> vertices, const BoostGraph& g);
};

std::ostream& operator<<(std::ostream& out, const Route& r);

#endif