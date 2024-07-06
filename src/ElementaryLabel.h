//
// Created by alberto on 15/04/24.
//

#ifndef _ELEMENTARY_LABEL_H
#define _ELEMENTARY_LABEL_H

#include "Graph.h"
#include <cstdlib>
#include <iostream>
#include <optional>
#include <map>

// enum class Accessibility : u_int32_t {
enum class Accessibility : uint32_t {
    NOT_ACCESSIBLE = 0u,
    ACCESSIBLE = 1u
};




struct ElementaryLabel {
    std::map<BoostVertex, Accessibility> accessibility{};// An ordered map mapping vertexes to accessibilitties
    std::optional<BoostVertex> second_last; // Track the second last visited vertex to aviod double loop
    double used_capacity;
    double cost;
};




// Defining operator "<<" to print stuffs for the above two objects
std::ostream& operator<<(std::ostream& out, const Accessibility& acc);
std::ostream& operator<<(std::ostream& out, const ElementaryLabel& lbl);



// Function to set the initial labels
ElementaryLabel get_initial_label(const Graph& i);
ElementaryLabel get_initial_label_incremental_resource();

// Function to add new resources
void add_resources(const std::set<size_t>& vertices, ElementaryLabel& l);

// Function to detect new resources

// Function to extend a path or check dominance
bool extend(const BoostGraph& bg, ElementaryLabel& lbl_new, const ElementaryLabel& lbl_old, BoostArc arc);
bool extend_incremental_resource(const BoostGraph& bg, ElementaryLabel& lbl_new, const ElementaryLabel& lbl_old, BoostArc arc);

bool dominance(const ElementaryLabel& l1, const ElementaryLabel& l2);

// operators
bool operator==(const ElementaryLabel& l1, const ElementaryLabel& l2);
bool operator<=(const ElementaryLabel& l1, const ElementaryLabel& l2);
bool operator<(const ElementaryLabel& l1, const ElementaryLabel& l2);

#endif
