//
//  main.cpp
//  DistributedSystem
//
//  Created by Aniket Bhushan on 31/12/19.
//  Copyright © 2019 Aniket Bhushan. All rights reserved.
//

#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/lockfree/queue.hpp>
#include <immintrin.h>

using namespace boost;

void way1()
{
    adjacency_list<> g;
    
    // Add 4 vertices to the graph
    adjacency_list<>::vertex_descriptor v1 = add_vertex(g);
    adjacency_list<>::vertex_descriptor v2 = add_vertex(g);
    adjacency_list<>::vertex_descriptor v3 = add_vertex(g);
    adjacency_list<>::vertex_descriptor v4 = add_vertex(g);
    
    // Get vertices
    typedef adjacency_list<>::vertex_iterator iterator;
    std::pair<iterator, iterator> p = vertices(g);
    
    // Print vertices : 0, 1, 2, 3
    for (auto it=p.first; it!=p.second; ++it) {
        std::cout << *it << std::endl;
    }
    
    // Print size_t
    std::cout << typeid(v1).name() << std::endl;
    
    // Connect the graph
    std::pair<adjacency_list<>::edge_descriptor, bool> ep;
    
    ep = add_edge(v1, v2, g);
    ep = add_edge(v2, v3, g);
    ep = add_edge(v3, v4, g);
    ep = add_edge(v4, v1, g);
    
    // Access and print the edges : (0, 1), (1, 2), (2, 3), (3, 0)
    auto edgep = edges(g);
    for (auto it=edgep.first; it!=edgep.second; ++it) {
        std::cout << *it << std::endl;
    }
}

void way2()
{
    // vecS : vector of selector
    adjacency_list<vecS, vecS, undirectedS> g; // node, edge
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    // adds edges without explicitly adding vertices
    add_edge(topLeft, topRight, g);
    add_edge(topRight, bottomRight, g);
    add_edge(bottomRight, bottomLeft, g);
    add_edge(bottomLeft, topLeft, g);
}

void NonNamedParameters()
{
    // vecS : vector of selector
    typedef adjacency_list<vecS, vecS, undirectedS> graph; // node, edge
    graph g;
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    // adds edges without explicitly adding vertices
    add_edge(topLeft, topRight, g);
    add_edge(topRight, bottomRight, g);
    add_edge(bottomRight, bottomLeft, g);
    add_edge(bottomLeft, topLeft, g);
    
    queue<graph::vertex_descriptor> q;
    
    // Null visitor which doesn't do anything
    bfs_visitor<null_visitor> vis;
    
    // Graph has 4 vertices - colormap needs 4 elements
    std::array<default_color_type, 4> colormap;
    
    // Pointer is automatically used as a property map
    breadth_first_search(g, topLeft, q, vis, colormap.data()); // graph, start, queue, vis, colorMap (visited or not)
}

void NamedParameters()
{
    // vecS : vector of selector
    typedef adjacency_list<vecS, vecS, undirectedS> graph; // node, edge
    graph g;
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    // adds edges without explicitly adding vertices
    add_edge(topLeft, topRight, g);
    add_edge(topRight, bottomRight, g);
    add_edge(bottomRight, bottomLeft, g);
    add_edge(bottomLeft, topLeft, g);
    
    queue<graph::vertex_descriptor> q;
    
    // Null visitor which doesn't do anything
    bfs_visitor<null_visitor> vis;
    
    // visitor() gives parameter a name
    breadth_first_search(g, topLeft, visitor(vis)); // graph, start, queue, vis, colorMap (visited or not)
}

// My own null visitor which doesn't do anything
struct my_null_visitor
{
    typedef on_no_event event_filter;
    template<class T, class Graph>
    void operator()(T, Graph&){}
};

void UserDefinedNullVisitor()
{
    // vecS : vector of selector
    typedef adjacency_list<vecS, vecS, undirectedS> graph; // node, edge
    graph g;
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    // adds edges without explicitly adding vertices
    add_edge(topLeft, topRight, g);
    add_edge(topRight, bottomRight, g);
    add_edge(bottomRight, bottomLeft, g);
    add_edge(bottomLeft, topLeft, g);
    
    // Null visitor which doesn't do anything
    bfs_visitor<my_null_visitor> vis;
    
    // visitor() gives parameter a name
    breadth_first_search(g, topLeft, visitor(vis)); // graph, start, queue, vis, colorMap (visited or not)
}

struct my_discover_visitor
{
    typedef on_discover_vertex event_filter;
    template <class T, class Graph>
    void operator()(T t, Graph& g) // T is vertex
    {
        std::cout << t << std::endl;
    }
};

void DiscoverVisitor()
{
    typedef adjacency_list<vecS, vecS, undirectedS> graph;
    graph g;
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    add_edge(topLeft, topRight, g);
    add_edge(topRight, bottomRight, g);
    add_edge(bottomRight, bottomLeft, g);
    add_edge(bottomLeft, topLeft, g);
    
    // 0, 1, 3, 2
    breadth_first_search(g, topLeft, visitor(make_bfs_visitor(my_discover_visitor())));
    
    std::cout << std::endl;
    
    // 0, 1, 2, 3
    depth_first_search(g, visitor(make_dfs_visitor(my_discover_visitor())));
}

void RecordingDistances()
{
    std::array<int, 4> distances = {{0}};
    
    typedef adjacency_list<vecS, vecS, undirectedS> graph;
    graph g;
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    add_edge(topLeft, topRight, g);
    add_edge(topRight, bottomRight, g);
    add_edge(bottomRight, bottomLeft, g);
    add_edge(bottomLeft, topLeft, g);
    
    
    // record_distance() to write distance to the property map
    // on_tree_edge is the event filter here
    breadth_first_search(g, topLeft, visitor(make_bfs_visitor(record_distances(distances.data(), on_tree_edge()))));
    
    // 0, 1, 2, 1
    for (auto d : distances) {
        std::cout << d <<  " ";
    }
    std::cout << std::endl;
}

void RecordingPredecessors()
{
    typedef adjacency_list<vecS, vecS, undirectedS> graph;
    graph g;
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    std::array<int, 4> predecessors;
    predecessors[bottomRight] = bottomRight;
    
    add_edge(topLeft, topRight, g);
    add_edge(topRight, bottomRight, g);
    add_edge(bottomRight, bottomLeft, g);
    add_edge(bottomLeft, topLeft, g);
    
    
    breadth_first_search(g, bottomRight, visitor(make_bfs_visitor(record_predecessors(predecessors.data(), on_tree_edge()))));
    
    int p = topLeft;
    while (p!=bottomRight) { // 0, 1, 2
        std::cout << p << " ";
        p = predecessors[p];
    }
    std::cout << p << " ";
    std::cout << std::endl;
}

void RecordDistancesAndPredecessors()
{
    typedef adjacency_list<vecS, vecS, undirectedS> graph;
    graph g;
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    std::array<int, 4> distances = {{0}};
    std::array<int, 4> predecessors;
    predecessors[bottomRight] = bottomRight;
    
    
    breadth_first_search(g, bottomRight, visitor(make_bfs_visitor(std::make_pair( record_distances(distances.data(), on_tree_edge()),
                                                                                 record_predecessors(predecessors.data(), on_tree_edge())))));
    
    //    std::cout << "Distances : ";
    //    for (auto d : distances) {
    //        std::cout << d <<  " ";
    //    }
    //    std::cout << std::endl;
    //
    //    std::cout << "Predecessors : ";
    //    int p = topLeft;
    //    while (p!=bottomRight) { // 0, 1, 2
    //        std::cout << p << " ";
    //        p = predecessors[p];
    //    }
    //    std::cout << p << " ";
    //    std::cout << std::endl;
    
}

void PropertyList()
{
    // Add weight as edge property
    typedef adjacency_list<vecS, vecS, undirectedS, no_property, property<edge_weight_t, int>> graph;
    typedef std::pair<int,int> E;
    
    
    std::array<std::pair<int,int>, 4> myedges = {
        E(1, 2), E(1,2), E(2,3), E(2,0)};
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    // ----
    std::array<int, 4> weights = {{2, 1, 1, 1}}; // 1st edge has weight 2
    graph g(myedges.begin(), myedges.end(), weights.data(), 4);
    
    // Instead of using weights we can use access property map and set weight per edge
    graph g1(myedges.begin(), myedges.end(), 4);
    property_map<graph, edge_weight_t>::type edge_weight_map = get(edge_weight_t(), g);
    auto it = edges(g1).first;
    put(edge_weight_map, *it, 2);
    put(edge_weight_map, *++it, 1);
    put(edge_weight_map, *++it, 1);
    put(edge_weight_map, *++it, 1);
    
    // ------
    
    std::array<int, 4> predecessors;
    
    dijkstra_shortest_paths(g, bottomRight, predecessor_map(predecessors.data()));
    
    std::cout << "Predecessors : ";
    int p = topLeft;
    while (p!=bottomRight) { // 0, 1, 2
        std::cout << p << " ";
        p = predecessors[p];
    }
    std::cout << p << " ";
    std::cout << std::endl;
}

void BundledProperties()
{
    struct edge_properties {int weight;};
    typedef adjacency_list<vecS, vecS, undirectedS, no_property, edge_properties> graph;
    typedef std::pair<int,int> E;
    enum {topLeft, topRight, bottomRight, bottomLeft};
    
    std::array<std::pair<int,int>, 4> myedges = {
        E(1, 0), E(1,2), E(2,3), E(3,0)};
    
    std::array<int, 4> predecessors;

    // ---
    std::array<edge_properties, 4> weights = {{2, 1, 1, 1}};
    graph g(myedges.begin(), myedges.end(), weights.data(), 4);
    
    graph g1(myedges.begin(), myedges.end(), 4);
    
    // Accessing property map and setting weight per edge
    auto it = edges(g1).first;
    g1[*it].weight = 2;
    g1[*++it].weight = 1;
    g1[*++it].weight = 1;
    g1[*++it].weight = 1;
    
    // --
    dijkstra_shortest_paths(g, bottomRight, predecessor_map(predecessors.data()).weight_map(get(&edge_properties::weight, g)));
    
    std::cout << "Predecessors : ";
    int p = topLeft;
    while (p!=bottomRight) { // 0, 1, 2
        std::cout << p << " ";
        p = predecessors[p];
    }
    std::cout << p << " ";
    std::cout << std::endl;
}

std::string get_cpu_name()
{
    uint32_t data[4] = {0};
    
    //_asm
    //{
    //    cpuid;
    //}
    
    return std::string((const char*)data);
}

int main(int argc, const char * argv[]) {
    
    // way1();
    // way2();
    
    // Algorithms use Property maps
    // Property attached to a vertex / edge that is used to draw extra information.
    //      Core vs specialized algorithm
    //      Named vs non-named algorithm
    //      Internal vs external properties
    //          Lists vs bundled
    
    //    NonNamedParameters();
    //    UserDefinedNullVisitor();
    // DiscoverVisitor();
    // RecordingDistances();
    // RecordingPredecessors();
    // RecordDistancesAndPredecessors();
    // PropertyList();
    // BundledProperties();
    
    return 0;
}
