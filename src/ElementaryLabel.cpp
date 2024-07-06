#include "ElementaryLabel.h"
#include <cassert>
#include <ranges>

std::ostream& operator<<(std::ostream& out, const Accessibility& acc) {
    if(acc == Accessibility::ACCESSIBLE) {
        out << "✅";
    } else {
        out << "❌";
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const ElementaryLabel& lbl) {
    out << "[cost=" << lbl.cost << ", used_cap=" << lbl.used_capacity << ", acc={";
    for(const auto& [v, a] : lbl.accessibility) {
        out << v << "=" << a << ",";
    }
    out << "\b}]";
    return out;
}



//////////////////////////////////////////////////////////////////////////////////
////////////////// Funciton to initialize labels for all custumors ///////////////
//////////////////////////////////////////////////////////////////////////////////
ElementaryLabel get_initial_label(const Graph& g) {
    auto l = ElementaryLabel{}; // Object to hold all initial labels for the whole graph

    for(auto v : g.customers()) { //  Loop through all customors
        l.accessibility[v] = Accessibility::ACCESSIBLE; // Setting all accessibility to accessible
    }

    l.second_last = std::nullopt;
    l.used_capacity = 0.0;
    l.cost = 0.0;

    return l;
}



ElementaryLabel get_initial_label_incremental_resource()
{
    auto l = ElementaryLabel{};
    l.second_last = std::nullopt;
    l.used_capacity = 0.0;
    l.cost = 0.0;
    return l;
}

void add_resources(const std::set<size_t>& vertices, ElementaryLabel& l)
{
    for(auto v : vertices) { //  Loop through all customors
        l.accessibility[v] = Accessibility::ACCESSIBLE; // Setting all accessibility to accessible
    }
    std::cout << "Current resource size is " << l.accessibility.size() << '\n';
}


//////////////////////////////////////////////////////////////////////////////////////
/////////////////// Function to extend a path ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool extend(const BoostGraph& bg, ElementaryLabel& lbl_new, const ElementaryLabel& lbl_old, BoostArc arc) {
    const auto& instance = bg[boost::graph_bundle];
    const auto dest = boost::target(arc, bg);

    if(dest == lbl_old.second_last)
    {
        return false;
    }

    if( not bg[dest].returning_depot and
        lbl_old.accessibility.at(dest) == Accessibility::NOT_ACCESSIBLE)
    {
        return false;
    }

    if(lbl_old.used_capacity + bg[dest].demand > instance.capacity) {
        return false;
    }

    lbl_new.accessibility = lbl_old.accessibility;

    if(not bg[dest].returning_depot) {
        lbl_new.accessibility[dest] = Accessibility::NOT_ACCESSIBLE;
    }

    lbl_new.second_last = boost::source(arc, bg);
    lbl_new.used_capacity = lbl_old.used_capacity + bg[dest].demand;
    lbl_new.cost = lbl_old.cost + bg[arc].cost;

    return true;
}



bool extend_incremental_resource(const BoostGraph& bg, ElementaryLabel& lbl_new, const ElementaryLabel& lbl_old, BoostArc arc) {
    // I should modify this funciton to include the case wher there is no accessbility associated with that vertex
    const auto& instance = bg[boost::graph_bundle];
    const auto dest = boost::target(arc, bg);

    if(dest == lbl_old.second_last)
    {
        return false;
    }

    // Check if the destination is accessible or if it's not in the map (assume it's accessible if not found)
    const auto it = lbl_old.accessibility.find(dest);
    const bool isAccessible = (it == lbl_old.accessibility.end()) || (it->second != Accessibility::NOT_ACCESSIBLE);
    const bool isFound = (it != lbl_old.accessibility.end());

    if(!bg[dest].returning_depot and !isAccessible)
    {
        return false;
    }

    if(lbl_old.used_capacity + bg[dest].demand > instance.capacity) {
        return false;
    }

    lbl_new.accessibility = lbl_old.accessibility;

    if(!bg[dest].returning_depot && isFound) {
        lbl_new.accessibility[dest] = Accessibility::NOT_ACCESSIBLE;
    }

    lbl_new.second_last = boost::source(arc, bg);
    lbl_new.used_capacity = lbl_old.used_capacity + bg[dest].demand;
    lbl_new.cost = lbl_old.cost + bg[arc].cost;

    return true;
}



//////////////////////////////////////////////////////////////////////////////////////
/////////////////// Function to check dominance ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool dominance(const ElementaryLabel& l1, const ElementaryLabel& l2) {
    if(l1.second_last != l2.second_last)
    {
        return false;
    }

    if(l1.cost > l2.cost) {
        return false;
    }
    if(l1.used_capacity > l2.used_capacity) {
        return false;
    }

    assert(l1.accessibility.size() == l2.accessibility.size());

    for(const auto& [cust, l1_acces] : l1.accessibility) { // Loop throgh pairs in the map
        const auto& l2_acces = l2.accessibility.at(cust); // Retrive the corresponding accessibilty for l2
        if( l2_acces == Accessibility::ACCESSIBLE and
            l1_acces == Accessibility::NOT_ACCESSIBLE)
        {
            return false;
        }
    }

    return true;
}



//////////////////////////////////////////////////////////////////////////////////////
/////////////////// Define operators for ElementaryLabel objects ////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/// I don't think these are used....////////////////////////////////////////////////
bool operator==(const ElementaryLabel& l1, const ElementaryLabel& l2) {
    constexpr auto EPS = 0.001;

    if(l1.second_last != l2.second_last)
    {
        return false;
    }

    if(std::abs(l1.cost - l2.cost) > EPS) {
        return false;
    }

    if(std::abs(l1.used_capacity - l2.used_capacity) > EPS) {
        return false;
    }

    return l1.accessibility == l2.accessibility;
}

bool operator<=(const ElementaryLabel& l1, const ElementaryLabel& l2) {
    return dominance(l1, l2);
}

bool operator<(const ElementaryLabel& l1, const ElementaryLabel& l2) {
    return dominance(l1, l2) and not (l1 == l2);
}