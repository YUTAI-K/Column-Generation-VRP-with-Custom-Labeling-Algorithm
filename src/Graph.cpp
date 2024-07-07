#include "Graph.h"
#include <boost/graph/adjacency_list.hpp>
#include <random>
#include <algorithm>
std::tuple<Graph, std::vector<double>, std::vector<double>>
generate_random_graph(size_t n_customers, unsigned int seed) {
    std::mt19937 generator{seed}; // selecting generator
    std::uniform_real_distribution<> uni{1.0, 11.0}; // unif(1,11)
    const auto n_vertices = n_customers + 2u; // the plus 2 are vertices of depots

    std::vector<double> xs(n_vertices), ys(n_vertices), demand(n_vertices);

    // [&] () { return uni(generator); } is a lambda function.
    // [&] captures all local variables
    std::generate(xs.begin(), xs.end(), [&] () { return uni(generator); });
    std::generate(ys.begin(), ys.end(), [&] () { return uni(generator); });
    std::generate(demand.begin(), demand.end(), [&] () { return uni(generator); });

    // No demand at the start of finish depot
    demand[0u] = 0.0;
    demand[n_vertices - 1u] = 0.0;

    const auto max_demand = *std::ranges::max_element(demand);
    const auto capacity = std::max(max_demand, 6.0 * static_cast<double>(n_vertices) / 4.0);

    Graph g;

    // The square brackets [] used with boost::graph_bundle in the context of Boost Graph Library (BGL)
    // represent the way to access properties associated with the entire graph, through what's called a property map.
    // boost::graph_bundle is a special key used with property maps when you want to access properties associated with the entire graph, not just individual vertices or edges.
    g.g[boost::graph_bundle] = Instance{capacity};

    for(size_t i = 0u; i < n_vertices; ++i) {
        const bool departing_depot = (i == 0u);// Set to true iff i == 0, this vertex is the first one
        const bool returning_depot = (i == n_vertices - 1u); // Set to true iff i == #vertices-1, this vertex is the last one
        g.vertices[i] = boost::add_vertex(   // (bundled vertex property, the graph you want to add)
                Vertex{departing_depot, returning_depot, demand[i]}, g.g);
    }

    size_t arc_id = 0u;
    for(size_t i = 0u; i < n_vertices - 1u; ++i) {
        for(size_t j = 1u; j < n_vertices; ++j) {
            if(i == j) { continue; } // No self connecting edges

            const auto cost = std::sqrt( // Compute distance as time cost, which is fixed
                    std::pow(xs[i] - xs[j], 2.0) + std::pow(ys[i] - ys[j], 2.0));
            auto [edge, success] = boost::add_edge( // Add edge i to j, with bundled property to the graph g.g
                    g.vertices[i], g.vertices[j], Arc{arc_id++, cost, cost}, g.g);

            assert(success);

            g.arcs[{i, j}] = edge;
        }
    }

    return {g, xs, ys};
}



void write_graphviz(const Graph& g, const std::string& filename) {
    std::ofstream ofs(filename);
    boost::write_graphviz(ofs, g.g,
        boost::make_label_writer(boost::get(&Vertex::demand, g.g)),
        boost::make_label_writer(boost::get(&Arc::cost, g.g)));
}




void print_grid(const Graph& g, const std::vector<double>& xs, const std::vector<double>& ys) {
    const int grid_size = 40;
    std::vector<std::vector<char>> grid(grid_size, std::vector<char>(grid_size, ' '));

    auto get_grid_pos = [&](double x, double y) {
        int gx = static_cast<int>((x / 11.0) * (grid_size - 1));
        int gy = static_cast<int>((y / 11.0) * (grid_size - 1));
        return std::pair<int, int>(gx, gy);
    };

    for (const auto& [key, vertex] : g.vertices) {
        auto [gx, gy] = get_grid_pos(xs[key], ys[key]);
        char mark = 'V';
        if (g.g[vertex].departing_depot) mark = 'D';
        else if (g.g[vertex].returning_depot) mark = 'R';
        grid[gy][gx] = mark;
    }

    // Print the legend
    std::cout << "Legend:\n";
    std::cout << "D: Departing Depot\n";
    std::cout << "R: Returning Depot\n";
    std::cout << "V: Customer\n\n";

    // Print the top border
    std::cout << '+' << std::string(grid_size, '-') << '+' << '\n';

    // Print the grid with side borders
    for (const auto& line : grid) {
        std::cout << '|';
        for (const auto& ch : line) {
            std::cout << ch;
        }
        std::cout << '|' << '\n';
    }

    // Print the bottom border
    std::cout << '+' << std::string(grid_size, '-') << '+' << '\n';
}

void print_graph(const Graph& g, const std::vector<double>& xs, const std::vector<double>& ys) {
    std::cout << "Graph Visualization:\n";
    print_grid(g, xs, ys);
    std::cout << '\n';
    for (const auto& [key, vertex] : g.vertices) {
        std::cout << "Vertex " << key << " (Demand: " << g.g[vertex].demand << "):\n";
        auto out_edges = boost::out_edges(vertex, g.g);
        for (auto edge_it = out_edges.first; edge_it != out_edges.second; ++edge_it) {
            auto target = boost::target(*edge_it, g.g);
            std::cout << "  -> Vertex " << target << " (Cost: " << g.g[*edge_it].cost << ")\n";
        }
    }
}