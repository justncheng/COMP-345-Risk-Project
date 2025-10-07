#include "Map.h"
#include <iostream>

void testLoadMaps() {
    std::cout << "=== Testing Map Loading and Validation ===\n";
    
    // Test with valid map files
    std::vector<std::string> testFiles = {
        "Chicago.map",
        "england.map", 
        "Florida.map"
    };
    
    for (const std::string& filename : testFiles) {
        std::cout << "\n--- Testing: " << filename << " ---" << std::endl;
        
        Map* map = MapLoader::loadMap(filename);
        
        if (map) {
            std::cout << "Successfully loaded map: " << *map << std::endl;
            std::cout << "Territories: " << map->getTerritories().size() << std::endl;
            std::cout << "Continents: " << map->getContinents().size() << std::endl;
            
            // Perform validation
            bool isValid = map->validate();
            std::cout << "Map validation: " << (isValid ? "PASS" : "FAIL") << std::endl;
            
            if (!isValid) {
                std::cout << "Detailed validation results:" << std::endl;
                std::cout << "  - Connected graph: " << (map->isConnectedGraph() ? "YES" : "NO") << std::endl;
                std::cout << "  - Connected continents: " << (map->continentsAreConnectedSubgraphs() ? "YES" : "NO") << std::endl;
                std::cout << "  - Each country in one continent: " << (map->eachCountryInOneContinent() ? "YES" : "NO") << std::endl;
            }
            
            delete map;
        } else {
            std::cout << "Failed to load map: " << filename << std::endl;
        }
    }
    
    // Test with invalid map file
    std::cout << "\n--- Testing with invalid map file ---" << std::endl;
    Map* invalidMap = MapLoader::loadMap("invalid.map");
    if (!invalidMap) {
        std::cout << "Correctly rejected invalid map file" << std::endl;
    }
    
    std::cout << "\n=== Map Testing Complete ===\n\n";
}
