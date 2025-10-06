#include "Map.h"
#include <iostream>

/**
 * @brief Test function to demonstrate map loading and validation
 */
void testLoadMaps() {
    std::cout << "=== Testing Map Loading and Validation ===" << std::endl;
    
    MapLoader loader;
    
    // Test with valid map files
    std::vector<std::string> testFiles = {
        "Chicago.map",
        "England.map", 
        "Florida.map"
    };
    
    for (const std::string& filename : testFiles) {
        std::cout << "\n--- Loading: " << filename << " ---" << std::endl;
        
        Map* map = loader.loadMap(filename);
        if (map) {
            std::cout << "Successfully loaded map!" << std::endl;
            std::cout << *map << std::endl;
            
            // Validate the map
            std::cout << "Validating map..." << std::endl;
            bool isValid = map->validate();
            
            std::cout << "Map validation results:" << std::endl;
            std::cout << "- Connected graph: " << (map->isConnectedGraph() ? "YES" : "NO") << std::endl;
            std::cout << "- Connected continents: " << (map->continentsAreConnectedSubgraphs() ? "YES" : "NO") << std::endl;
            std::cout << "- Territories in one continent: " << (map->territoriesBelongToOneContinent() ? "YES" : "NO") << std::endl;
            std::cout << "- Overall valid: " << (isValid ? "YES" : "NO") << std::endl;
            
            delete map;
        } else {
            std::cout << "Failed to load map!" << std::endl;
        }
    }
    
    std::cout << "\n=== Map Testing Complete ===" << std::endl;
}
