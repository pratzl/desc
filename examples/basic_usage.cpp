/**
 * @file basic_usage.cpp
 * @brief Basic usage example for vertex descriptors
 */

#include <desc/vertex_descriptor.hpp>
#include <desc/vertex_descriptor_view.hpp>
#include <iostream>
#include <vector>
#include <map>

int main() {
    // Example 1: Using vertex descriptors with vector (random access)
    std::cout << "=== Vector Example ===\n";
    std::vector<int> vertices = {10, 20, 30, 40, 50};
    
    using VectorIter = std::vector<int>::iterator;
    using VD_Vector = desc::vertex_descriptor<VectorIter>;
    
    VD_Vector vd{2};  // Refers to index 2
    std::cout << "Vertex ID: " << vd.vertex_id() << "\n";
    std::cout << "Value: " << vd.value() << "\n";
    
    // Create view and iterate
    desc::vertex_descriptor_view view{vertices};
    std::cout << "All vertices:\n";
    for (auto desc : view) {
        std::cout << "  Vertex " << desc.vertex_id() << "\n";
    }
    
    // Example 2: Using vertex descriptors with map (bidirectional)
    std::cout << "\n=== Map Example ===\n";
    std::map<int, std::string> vertex_map = {
        {100, "Node A"},
        {200, "Node B"},
        {300, "Node C"}
    };
    
    using MapIter = std::map<int, std::string>::iterator;
    using VD_Map = desc::vertex_descriptor<MapIter>;
    
    auto it = vertex_map.find(200);
    VD_Map vd_map{it};
    std::cout << "Vertex ID: " << vd_map.vertex_id() << "\n";
    
    // Create view and iterate
    desc::vertex_descriptor_view map_view{vertex_map};
    std::cout << "All vertices:\n";
    for (auto desc : map_view) {
        std::cout << "  Vertex " << desc.vertex_id() << "\n";
    }
    
    return 0;
}
