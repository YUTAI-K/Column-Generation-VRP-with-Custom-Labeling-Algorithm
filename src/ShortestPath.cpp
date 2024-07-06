#include "ShortestPath.h"
#include "ElementaryLabel.h"
#include <algorithm>
#include <boost/graph/r_c_shortest_paths.hpp>
#include <cassert>
#include <ranges>
#include <vector>
#include <numeric>

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// Function to initialize the routes ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Route> ShortestPathSolver::solve_ignoring_shortest_path(const std::map<BoostVertex, double>& duals)
{
    //boost::edges(g.g) returns a pair of iterators pointing to the start and end of the edge list.
    for(auto [it, it_end] = boost::edges(g.g); it != it_end; ++it) { // Loop through all edges
        // Edge and Target Vertex: For each edge,
        // it retrieves the target vertex using boost::target(edge, g.g),
        // which gives the endpoint of the edge in the directed graph.
        const auto& edge = *it;
        const auto& destination = boost::target(edge, g.g);

        // Skipping Depots: If the target vertex represents a returning depot, the loop skips further processing for this edge
        if(g.g[destination].returning_depot) {
            continue;
        }

        assert(not g.g[destination].departing_depot);

        // The cost of the edge is adjusted by subtracting the dual value associated with the destination vertex from the original cost.
        g.g[edge].cost = g.g[edge].original_cost - duals.at(destination);
    }

    // Property Maps: These lines get property maps for vertex indices and edge IDs which are necessary for the shortest path algorithm to reference vertices and edges properly.
    auto vertex_id_prop = boost::get(boost::vertex_index, g.g);
    auto edge_id_prop = boost::get(&Arc::id, g.g);

    // Containers for Results: These vectors will store the edges and labels of optimal paths found by the shortest path algorithm.
    std::vector<std::vector<BoostArc>> opt_path_edges;
    std::vector<ElementaryLabel> opt_path_labels;

    // Set initial labels
    auto l = get_initial_label_incremental_resource();

    boost::r_c_shortest_paths(
        g.g, // Graph
        vertex_id_prop, // Vertex property map
        edge_id_prop, // Edge property map
        g.departing_depot(), // Source vertex
        g.returning_depot(), // Sink vertex
        opt_path_edges, // Each non-dominated path edges will go here
        opt_path_labels, // Each non-dominated final path labels will go here
        l, // Starting label
        extend_incremental_resource, // Extension function
        dominance, // Dominance function
        boost::default_r_c_shortest_paths_allocator(),
        boost::default_r_c_shortest_paths_visitor());

    static constexpr auto EPS = 0.001;
    std::vector<std::vector<BoostArc>> edges_to_return{};
    std::vector<ElementaryLabel> labels_to_return{};
    for (auto [eds, lbl]: std::views::zip(opt_path_edges, opt_path_labels))
    {
        if (lbl.cost > -EPS)
            continue;
        auto isLooping = check_existence_of_loops(eds, lbl);
        if(!isLooping)
        {
            edges_to_return.push_back(eds);
            labels_to_return.push_back(lbl);
        }

    }

    if (!edges_to_return.empty())
    {
        // std::cout<< "Reduced costs are: ";
        // for (auto label: labels_to_return)
        //     std::cout << "(" << label << ")";
        // std::cout<<'\n';
        return routes_from_paths(edges_to_return, labels_to_return);
    }

    return routes_from_paths(edges_to_return, labels_to_return);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// Method to solve the second stage problem with whole vector of resources /////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Route> ShortestPathSolver::solve_shortest_path(const std::map<BoostVertex, double>& duals) {
    //boost::edges(g.g) returns a pair of iterators pointing to the start and end of the edge list.
    for(auto [it, it_end] = boost::edges(g.g); it != it_end; ++it) { // Loop through all edges
        // Edge and Target Vertex: For each edge,
        // it retrieves the target vertex using boost::target(edge, g.g),
        // which gives the endpoint of the edge in the directed graph.
        const auto& edge = *it;
        const auto& destination = boost::target(edge, g.g);

        // Skipping Depots: If the target vertex represents a returning depot, the loop skips further processing for this edge
        if(g.g[destination].returning_depot) {
            continue;
        }

        assert(not g.g[destination].departing_depot);

        // The cost of the edge is adjusted by subtracting the dual value associated with the destination vertex from the original cost.
        g.g[edge].cost = g.g[edge].original_cost - duals.at(destination);
    }

    // Containers for Results: These vectors will store the edges and labels of optimal paths found by the shortest path algorithm.
    std::vector<std::vector<BoostArc>> opt_path_edges;
    std::vector<ElementaryLabel> opt_path_labels;

    // Property Maps: These lines get property maps for vertex indices and edge IDs which are necessary for the shortest path algorithm to reference vertices and edges properly.
    auto vertex_id_prop = boost::get(boost::vertex_index, g.g);
    auto edge_id_prop = boost::get(&Arc::id, g.g);

    boost::r_c_shortest_paths(
            g.g, // Graph
            vertex_id_prop, // Vertex property map
            edge_id_prop, // Edge property map
            g.departing_depot(), // Source vertex
            g.returning_depot(), // Sink vertex
            opt_path_edges, // Each non-dominated path edges will go here
            opt_path_labels, // Each non-dominated final path labels will go here
            get_initial_label(g), // Starting label
            extend, // Extension function
            dominance, // Dominance function
            boost::default_r_c_shortest_paths_allocator(),
            boost::default_r_c_shortest_paths_visitor());

    return routes_from_paths(opt_path_edges, opt_path_labels);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// Method to solve the second stage problem with incrementing vector of resources //////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Route> ShortestPathSolver::solve_incremental_shortest_path(const std::map<BoostVertex, double>& duals)
{
    std::cout << " ========================================================== " << '\n'
              << " === You are using the incremental shortest path method === " << '\n'
              << " ========================================================== " << '\n';

    //boost::edges(g.g) returns a pair of iterators pointing to the start and end of the edge list.
    for(auto [it, it_end] = boost::edges(g.g); it != it_end; ++it) { // Loop through all edges
        // Edge and Target Vertex: For each edge,
        // it retrieves the target vertex using boost::target(edge, g.g),
        // which gives the endpoint of the edge in the directed graph.
        const auto& edge = *it;
        const auto& destination = boost::target(edge, g.g);

        // Skipping Depots: If the target vertex represents a returning depot, the loop skips further processing for this edge
        if(g.g[destination].returning_depot) {
            continue;
        }

        assert(not g.g[destination].departing_depot);

        // The cost of the edge is adjusted by subtracting the dual value associated with the destination vertex from the original cost.
        g.g[edge].cost = g.g[edge].original_cost - duals.at(destination);
    }

    // Property Maps: These lines get property maps for vertex indices and edge IDs which are necessary for the shortest path algorithm to reference vertices and edges properly.
    auto vertex_id_prop = boost::get(boost::vertex_index, g.g);
    auto edge_id_prop = boost::get(&Arc::id, g.g);

    // Containers for Results: These vectors will store the edges and labels of optimal paths found by the shortest path algorithm.
    std::vector<std::vector<BoostArc>> opt_path_edges;
    std::vector<ElementaryLabel> opt_path_labels;

    // A number to track the number of vertices added in each iteration
    int number_of_node_resources_added{0};

    // Set initial labels
    auto l = get_initial_label_incremental_resource();

    while (true)
    {
        // Clear vectors at the start of each iteration
        opt_path_edges.clear();
        opt_path_labels.clear();

        boost::r_c_shortest_paths(
                g.g, // Graph
                vertex_id_prop, // Vertex property map
                edge_id_prop, // Edge property map
                g.departing_depot(), // Source vertex
                g.returning_depot(), // Sink vertex
                opt_path_edges, // Each non-dominated path edges will go here
                opt_path_labels, // Each non-dominated final path labels will go here
                l, // Starting label
                extend_incremental_resource, // Extension function
                dominance, // Dominance function
                boost::default_r_c_shortest_paths_allocator(),
                boost::default_r_c_shortest_paths_visitor());

        auto vertices_to_add = check_negative_loops(opt_path_edges, opt_path_labels);

        if (vertices_to_add.size() == 0)
            break;

        number_of_node_resources_added += vertices_to_add.size(); // Update the number of vertices added

        std::cout << "\nAdd resources for vertices: " << '\n';
        for(auto vertex:vertices_to_add)
        {
            std::cout << vertex << ' ';
        }
        std::cout << '\n';

        add_resources(vertices_to_add, l);
    }

    std::cout<< "Total number of node resources added: " << number_of_node_resources_added << '\n';

    return routes_from_paths(opt_path_edges, opt_path_labels);
}



std::vector<Route> ShortestPathSolver::solve_incremental_k_best_shortest_path(const std::map<BoostVertex, double>& duals, int k)
{
        std::cout << " ========================================================== " << '\n'
              << " === You are using the k-best incremental shortest path method === " << '\n'
              << " ========================================================== " << '\n';

    //boost::edges(g.g) returns a pair of iterators pointing to the start and end of the edge list.
    std::size_t num_negative_arc{0};

    for(auto [it, it_end] = boost::edges(g.g); it != it_end; ++it) { // Loop through all edges
        // Edge and Target Vertex: For each edge,
        // it retrieves the target vertex using boost::target(edge, g.g),
        // which gives the endpoint of the edge in the directed graph.
        const auto& edge = *it;
        const auto& destination = boost::target(edge, g.g);

        // Skipping Depots: If the target vertex represents a returning depot, the loop skips further processing for this edge
        if(g.g[destination].returning_depot) {
            continue;
        }

        assert(not g.g[destination].departing_depot);

        // The cost of the edge is adjusted by subtracting the dual value associated with the destination vertex from the original cost.
        g.g[edge].cost = g.g[edge].original_cost - duals.at(destination);
        if (g.g[edge].cost < 0)
            ++num_negative_arc;
    }
    std::size_t numArcs = boost::num_edges(g.g);
    double propotion_negative_arcs {static_cast<double>(num_negative_arc) / static_cast<double>(numArcs)};
    std::cout << "Propotion of negative edges is: "<< propotion_negative_arcs << '\n';

    // Property Maps: These lines get property maps for vertex indices and edge IDs which are necessary for the shortest path algorithm to reference vertices and edges properly.
    auto vertex_id_prop = boost::get(boost::vertex_index, g.g);
    auto edge_id_prop = boost::get(&Arc::id, g.g);

    // Containers for Results: These vectors will store the edges and labels of optimal paths found by the shortest path algorithm.
    std::vector<std::vector<BoostArc>> opt_path_edges;
    std::vector<ElementaryLabel> opt_path_labels;

    // Set initial labels
    auto l = get_initial_label_incremental_resource();

    while (true)
    {
        // Clear vectors at the start of each iteration
        opt_path_edges.clear();
        opt_path_labels.clear();

        boost::r_c_shortest_paths(
                g.g, // Graph
                vertex_id_prop, // Vertex property map
                edge_id_prop, // Edge property map
                g.departing_depot(), // Source vertex
                g.returning_depot(), // Sink vertex
                opt_path_edges, // Each non-dominated path edges will go here
                opt_path_labels, // Each non-dominated final path labels will go here
                l, // Starting label
                extend_incremental_resource, // Extension function
                dominance, // Dominance function
                boost::default_r_c_shortest_paths_allocator(),
                boost::default_r_c_shortest_paths_visitor());

        auto vertices_to_add = check_negative_loops(opt_path_edges, opt_path_labels);

        if (vertices_to_add.size() == 0)
            break;


        std::cout << "\nAdd resources for vertices: " << '\n';
        for(auto vertex:vertices_to_add)
        {
            std::cout << vertex << ' ';
        }
        std::cout << '\n';

        add_resources(vertices_to_add, l);
    }

    return routes_from_paths_only_biggest(opt_path_edges, opt_path_labels, k);
}




//////////////////////////////////////////////////////////////////////////////////////////////
///////////// Method to solve the problem with a ignoring-incremental algorithm //////////////
//////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Route> ShortestPathSolver::solve_ignoring_incremental_shortest_path(const std::map<BoostVertex, double>& duals)
{
    //boost::edges(g.g) returns a pair of iterators pointing to the start and end of the edge list.
    for(auto [it, it_end] = boost::edges(g.g); it != it_end; ++it) { // Loop through all edges
        // Edge and Target Vertex: For each edge,
        // it retrieves the target vertex using boost::target(edge, g.g),
        // which gives the endpoint of the edge in the directed graph.
        const auto& edge = *it;
        const auto& destination = boost::target(edge, g.g);

        // Skipping Depots: If the target vertex represents a returning depot, the loop skips further processing for this edge
        if(g.g[destination].returning_depot) {
            continue;
        }

        assert(not g.g[destination].departing_depot);

        // The cost of the edge is adjusted by subtracting the dual value associated with the destination vertex from the original cost.
        g.g[edge].cost = g.g[edge].original_cost - duals.at(destination);
    }

    // Property Maps: These lines get property maps for vertex indices and edge IDs which are necessary for the shortest path algorithm to reference vertices and edges properly.
    auto vertex_id_prop = boost::get(boost::vertex_index, g.g);
    auto edge_id_prop = boost::get(&Arc::id, g.g);

    // Containers for Results: These vectors will store the edges and labels of optimal paths found by the shortest path algorithm.
    std::vector<std::vector<BoostArc>> opt_path_edges;
    std::vector<ElementaryLabel> opt_path_labels;

    // Set initial labels
    auto l = get_initial_label_incremental_resource();

    boost::r_c_shortest_paths(
        g.g, // Graph
        vertex_id_prop, // Vertex property map
        edge_id_prop, // Edge property map
        g.departing_depot(), // Source vertex
        g.returning_depot(), // Sink vertex
        opt_path_edges, // Each non-dominated path edges will go here
        opt_path_labels, // Each non-dominated final path labels will go here
        l, // Starting label
        extend_incremental_resource, // Extension function
        dominance, // Dominance function
        boost::default_r_c_shortest_paths_allocator(),
        boost::default_r_c_shortest_paths_visitor());

    auto vertices_to_add = check_negative_loops(opt_path_edges, opt_path_labels);

    if (vertices_to_add.size() == 0) // Just in case this happens, it shouldn't, generally speaking
        return routes_from_paths(opt_path_edges, opt_path_labels);

    std::cout << "\nAdd resources for vertices: " << '\n';
    for(auto vertex:vertices_to_add)
    {
        std::cout << vertex << ' ';
    }
    std::cout << '\n';

    add_resources(vertices_to_add, l);


    while (true)
    {
        // Clear vectors at the start of each iteration
        opt_path_edges.clear();
        opt_path_labels.clear();

        boost::r_c_shortest_paths(
                g.g, // Graph
                vertex_id_prop, // Vertex property map
                edge_id_prop, // Edge property map
                g.departing_depot(), // Source vertex
                g.returning_depot(), // Sink vertex
                opt_path_edges, // Each non-dominated path edges will go here
                opt_path_labels, // Each non-dominated final path labels will go here
                l, // Starting label
                extend_incremental_resource, // Extension function
                dominance, // Dominance function
                boost::default_r_c_shortest_paths_allocator(),
                boost::default_r_c_shortest_paths_visitor());
        // Check if there are routes without duplicated vertices, if there is, return them
        static constexpr auto EPS = 0.001;
        std::vector<std::vector<BoostArc>> edges_to_return{};
        std::vector<ElementaryLabel> labels_to_return{};
        for (auto [eds, lbl]: std::views::zip(opt_path_edges, opt_path_labels))
        {// FOr all edges, if it has a negative cost and is not looping, we want to return such thing
            if (lbl.cost > - EPS)
                continue;
            auto isLooping = check_existence_of_loops(eds, lbl);
            if(!isLooping)
            {
                edges_to_return.push_back(eds);
                labels_to_return.push_back(lbl);
            }
        }
        if (!edges_to_return.empty())
        {
            return routes_from_paths(edges_to_return, labels_to_return);
        }


        // IF there are routes containing duplicated vertices, add them to label pool
        vertices_to_add = check_negative_loops(opt_path_edges, opt_path_labels);

        if (vertices_to_add.size() == 0)
            break;

        std::cout << "\nAdd resources for vertices: " << '\n';
        for(auto vertex:vertices_to_add)
        {
            std::cout << vertex << ' ';
        }
        std::cout << '\n';

        add_resources(vertices_to_add, l);
    }

    return routes_from_paths(opt_path_edges, opt_path_labels);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Method to solve a ignoring-incremental-multiplicity_spp ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Route> ShortestPathSolver::solve_ignoring_incremental_multiplicity_shortest_path(const std::map<BoostVertex, double>& duals)
{
        //boost::edges(g.g) returns a pair of iterators pointing to the start and end of the edge list.
    for(auto [it, it_end] = boost::edges(g.g); it != it_end; ++it) { // Loop through all edges
        // Edge and Target Vertex: For each edge,
        // it retrieves the target vertex using boost::target(edge, g.g),
        // which gives the endpoint of the edge in the directed graph.
        const auto& edge = *it;
        const auto& destination = boost::target(edge, g.g);

        // Skipping Depots: If the target vertex represents a returning depot, the loop skips further processing for this edge
        if(g.g[destination].returning_depot) {
            continue;
        }

        assert(not g.g[destination].departing_depot);

        // The cost of the edge is adjusted by subtracting the dual value associated with the destination vertex from the original cost.
        g.g[edge].cost = g.g[edge].original_cost - duals.at(destination);
    }

    // Property Maps: These lines get property maps for vertex indices and edge IDs which are necessary for the shortest path algorithm to reference vertices and edges properly.
    auto vertex_id_prop = boost::get(boost::vertex_index, g.g);
    auto edge_id_prop = boost::get(&Arc::id, g.g);

    // Containers for Results: These vectors will store the edges and labels of optimal paths found by the shortest path algorithm.
    std::vector<std::vector<BoostArc>> opt_path_edges;
    std::vector<ElementaryLabel> opt_path_labels;

    // Set initial labels
    auto l = get_initial_label_incremental_resource();

    boost::r_c_shortest_paths(
        g.g, // Graph
        vertex_id_prop, // Vertex property map
        edge_id_prop, // Edge property map
        g.departing_depot(), // Source vertex
        g.returning_depot(), // Sink vertex
        opt_path_edges, // Each non-dominated path edges will go here
        opt_path_labels, // Each non-dominated final path labels will go here
        l, // Starting label
        extend_incremental_resource, // Extension function
        dominance, // Dominance function
        boost::default_r_c_shortest_paths_allocator(),
        boost::default_r_c_shortest_paths_visitor());

    auto vertices_to_add = check_negative_loops_and_multiplicity(opt_path_edges, opt_path_labels);

    if (vertices_to_add.size() == 0) // Just in case this happens, it shouldn't, generally speaking
        return routes_from_paths(opt_path_edges, opt_path_labels);

    std::cout << "\nAdd resources for vertices: " << '\n';
    for(auto vertex:vertices_to_add)
    {
        std::cout << vertex << ' ';
    }
    std::cout << '\n';

    add_resources(vertices_to_add, l);


    while (true)
    {
        // Clear vectors at the start of each iteration
        opt_path_edges.clear();
        opt_path_labels.clear();

        boost::r_c_shortest_paths(
                g.g, // Graph
                vertex_id_prop, // Vertex property map
                edge_id_prop, // Edge property map
                g.departing_depot(), // Source vertex
                g.returning_depot(), // Sink vertex
                opt_path_edges, // Each non-dominated path edges will go here
                opt_path_labels, // Each non-dominated final path labels will go here
                l, // Starting label
                extend_incremental_resource, // Extension function
                dominance, // Dominance function
                boost::default_r_c_shortest_paths_allocator(),
                boost::default_r_c_shortest_paths_visitor());
        // Check if there are routes without duplicated vertices, if there is, return them
        static constexpr auto EPS = 0.001;
        std::vector<std::vector<BoostArc>> edges_to_return{};
        std::vector<ElementaryLabel> labels_to_return{};
        for (auto [eds, lbl]: std::views::zip(opt_path_edges, opt_path_labels))
        {
            if (lbl.cost > -EPS)
                continue;
            auto isLooping = check_existence_of_loops(eds, lbl);
            if(!isLooping)
            {
                edges_to_return.push_back(eds);
                labels_to_return.push_back(lbl);
            }
        }
        if (!edges_to_return.empty())
        {
            // std::cout<< "Reduced costs are: ";
            // for (auto label: labels_to_return)
            //     std::cout << "(" << label << ")";
            // std::cout<<'\n';
            return routes_from_paths(edges_to_return, labels_to_return);
        }


        // IF there all routes contains duplicated vertices, add them to label pool
        vertices_to_add = check_negative_loops_and_multiplicity(opt_path_edges, opt_path_labels);

        if (vertices_to_add.size() == 0)
            break;

        std::cout << "\nAdd resources for vertices: " << '\n';
        for(auto vertex:vertices_to_add)
        {
            std::cout << vertex << ' ';
        }
        std::cout << '\n';

        add_resources(vertices_to_add, l);
    }

    return routes_from_paths(opt_path_edges, opt_path_labels);
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// Method to create the routes to be return(with negative cost) //////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Route> ShortestPathSolver::routes_from_paths(
        const std::vector<std::vector<BoostArc>>& edges, // Matrix of arcs
        const std::vector<ElementaryLabel>& labels) const // // Vector of labels
{
    static constexpr auto EPS = 0.001;
    std::vector<Route> routes;

    assert(edges.size() == labels.size());

    // zip_view is a range adaptor that takes one or more views, and produces a view whose ith element is a tuple-like value consisting of the ith elements of all views. The size of produced view is the minimum of sizes of all adapted views.
    for(const auto& [eds, lbl] : std::views::zip(edges, labels)) {
        assert(boost::target(eds.front(), g.g) == g.returning_depot());
        assert(boost::source(eds.back(), g.g) == g.departing_depot());

        if(lbl.cost > -EPS) { // Filter to include only negative costs
            continue;
        }

        std::vector<size_t> vertices;
        vertices.push_back(g.returning_depot());

        for(auto e : eds) {
            vertices.push_back(boost::source(e, g.g));
        }

        assert(vertices.back() == g.departing_depot());
        std::reverse(vertices.begin(), vertices.end());
        routes.emplace_back(vertices, g.g);
    }

    return routes;
}



std::vector<Route> ShortestPathSolver::routes_from_paths_only_biggest(
        std::vector<std::vector<BoostArc>>& edges, // Matrix of arcs
        std::vector<ElementaryLabel>& labels,
        int k) const // // Vector of labels
{
    static constexpr auto EPS = 0.001;
    std::vector<Route> routes;
    assert(edges.size() == labels.size());

    std::cout << " ========================================================== " << '\n'
      << " === You will only return the "<< k <<" best routes === " << '\n'
      << " ========================================================== " << '\n';

    // Sort the labels by cost
    const auto edge_label_pairs = sort_routes_by_cost(edges, labels);

    // Now proceed with the zip and processing
    int limit {k};
    int idx{0};
    for (const auto& [eds, lbl] : std::views::zip(edge_label_pairs.first, edge_label_pairs.second)) {
        if(lbl.cost > -EPS) { // Filter to include only negative costs
            break; // If the cost is already bigger than 0, then all following labels will all be bigger than 0
        }

        std::vector<size_t> vertices;
        vertices.push_back(g.returning_depot());

        for(auto e : eds) {
            vertices.push_back(boost::source(e, g.g));
        }

        assert(vertices.back() == g.departing_depot());
        std::reverse(vertices.begin(), vertices.end());
        routes.emplace_back(vertices, g.g);

        ++idx;
        if (idx >= limit)
            break;
    }

    return routes;
}




/////////////////////////////////////////////////////////////////////////////
///////////////// Method to check for negative loops ////////////////////////
/////////////////////////////////////////////////////////////////////////////

std::set<size_t> ShortestPathSolver::check_negative_loops_in_one_path(
    const std::vector<BoostArc>& edges,
    const ElementaryLabel& label
) const
{
    constexpr auto EPS = 0.001;

    if(label.cost > -EPS)
    {
        return {};
    }

    std::set<size_t> seen;
    std::set<size_t> duplicates;

    // std::cerr << "\tCustomers in path: ";

    for (auto e : edges)
    {
        size_t sourceVertex = boost::source(e, g.g);

        if(sourceVertex == g.departing_depot())
        {
            break;
        }

        // std::cerr << sourceVertex << " ";

        const auto [_, unseen] = seen.insert(sourceVertex);

        if(not unseen)
        {
            duplicates.insert(sourceVertex);
        }
    }

    // std::cerr << "\n\tDuplicates: ";
    // for(auto d : duplicates)
    // {
    //     std::cerr << d << " ";
    // }
    // std::cerr << "\n";

    return duplicates;
}

std::set<size_t> ShortestPathSolver::check_negative_loops_and_multiplicity(
        const std::vector<std::vector<BoostArc>>& edges, // Matrix of arcs
        const std::vector<ElementaryLabel>& labels) const
{
    std::set<size_t> duplicates;
    std::map<size_t, int> multiplicity_map;
    for (const auto& [eds, lbl] : std::views::zip(edges, labels))
    {
        const auto vertices_dups = check_negative_loops_in_one_path(eds, lbl);
        for(auto vertex: vertices_dups)
        {
            ++multiplicity_map[vertex];
        }
    }
    // Now use a loop to detec the highest values
    int highest_multiplicity {0};
    size_t customer_to_add{0};  // 0 is where the starting depot is;
    for (auto [customer, multiplicity]: multiplicity_map)
    {
        if (multiplicity > highest_multiplicity)
        {
            highest_multiplicity = multiplicity;
            customer_to_add = customer;
        }
    }
    if (customer_to_add==0) // Which means we didn't find anything
        return duplicates;

    duplicates.insert(customer_to_add);
    std::cout << "The first customer with the biggest multiplicity is found, it is customer " << customer_to_add
    << " with multiplicity " << highest_multiplicity;
    return duplicates;
}

std::set<size_t> ShortestPathSolver::check_negative_loops( // This function only returns the vertices with biggest multiplicity
    const std::vector<std::vector<BoostArc>>& edges, // Matrix of arcs
    const std::vector<ElementaryLabel>& labels) const // // Vector of labels
{
    std::set<size_t> duplicates;

    for (const auto& [eds, lbl] : std::views::zip(edges, labels))
    {
        const auto path_dups = check_negative_loops_in_one_path(eds, lbl);
        duplicates.insert(path_dups.begin(), path_dups.end());
    }

    return duplicates;
}




// std::set<size_t> ShortestPathSolver::check_negative_loops_only_smallest(
//     std::vector<std::vector<BoostArc>>& edges, // Matrix of arcs
//     std::vector<ElementaryLabel>& labels)
// {
//     std::set<size_t> duplicates;
//
//     // Sort the edge and labels by cost
//     auto edge_label_pairs = sort_routes_by_cost(edges, labels);
//
//     int limit = 5;
//     int idx = 0;
//     for (const auto& [eds, lbl] : std::views::zip(edges, labels))
//     {
//         const auto path_dups = check_negative_loops_in_one_path(eds, lbl);
//         duplicates.insert(path_dups.begin(), path_dups.end());
//
//         ++idx;
//         if(idx >= limit)
//         {
//             break;
//         }
//     }
//
//     return duplicates;
// }



////////////////////////////////////////////////////////////////////////////
////////////// Method to sort the routes by costs //////////////////////////
////////////////////////////////////////////////////////////////////////////
// Returning a view is dangerous and can result in undefined behaviours
std::pair<std::vector<std::vector<BoostArc>>, std::vector<ElementaryLabel>>
    ShortestPathSolver::sort_routes_by_cost(
        std::vector<std::vector<BoostArc>>& edges,
        std::vector<ElementaryLabel>& labels) const
{
    // Create and initialize an index vector
    std::vector<size_t> indices(labels.size()); // Create an empty index vector of the same size as the labels
    std::iota(indices.begin(), indices.end(), 0); // Fill the range with increasing value, starting from 0, eahc time fills ++val

    // Sort indices based on comparing labels by cost
    std::sort(indices.begin(), indices.end(), // Range you want to sort
              [&labels](size_t i1, size_t i2) { return labels[i1].cost < labels[i2].cost; }); // Function to detect the order

    // Create temporary vectors to hold the sorted values
    std::vector<std::vector<BoostArc>> sortedEdges(edges.size());
    std::vector<ElementaryLabel> sortedLabels(labels.size());

    for (size_t i = 0; i < indices.size(); ++i) {
        sortedEdges[i] = std::move(edges[indices[i]]);  // Use std::move to avoid copying
        sortedLabels[i] = std::move(labels[indices[i]]);
    }

    return {std::move(sortedEdges), std::move(sortedLabels)};
}




/////////////////////////////////////////////////////////////////////////////////
/////////////////// Simple function return true if there are duplicate vertex //
//////////////////////////////////////////////////////////////////////////////////
bool ShortestPathSolver::check_existence_of_loops(
const std::vector<BoostArc>& edges,
const ElementaryLabel& label) const
{
    constexpr auto EPS = 0.001;

    if(label.cost > -EPS)
    {
        return true; // I don't want to include routes with positive costs
    }

    std::set<size_t> seen;


    for (auto e : edges)
    {
        size_t sourceVertex = boost::source(e, g.g);

        if(sourceVertex == g.departing_depot())
        {
            break;
        }


        const auto [_, unseen] = seen.insert(sourceVertex);
        // If insertion failed, it means we encountered a loop
        if(not unseen)
        {
            return true;
        }
    }

    return false;
}