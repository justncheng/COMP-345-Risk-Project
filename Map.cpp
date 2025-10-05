#include "Map.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stack>

// Territory class implementation

Territory::Territory(const std::string& name, const std::string& continent) 
    : name(name), armyUnits(0), owner(nullptr), continent(continent) {}

Territory::Territory(const Territory& other) 
    : name(other.name), armyUnits(other.armyUnits), owner(other.owner), 
      continent(other.continent) {
    // Deep copy adjacent territories
    for (Territory* adj : other.adjacentTerritories) {
        adjacentTerritories.push_back(adj);
    }
}

Territory::~Territory() {
    // Territories are managed by Map, so we don't delete adjacent territories here
    adjacentTerritories.clear();
}

Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        name = other.name;
        armyUnits = other.armyUnits;
        owner = other.owner;
        continent = other.continent;
        
        adjacentTerritories.clear();
        for (Territory* adj : other.adjacentTerritories) {
            adjacentTerritories.push_back(adj);
        }
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Territory& territory) {
    os << "Territory: " << territory.name 
       << " (Continent: " << territory.continent 
       << ", Armies: " << territory.armyUnits << ")";
    return os;
}

// Territory getters
std::string Territory::getName() const { return name; }
int Territory::getArmyUnits() const { return armyUnits; }
Player* Territory::getOwner() const { return owner; }
std::string Territory::getContinent() const { return continent; }
const std::vector<Territory*>& Territory::getAdjacentTerritories() const { 
    return adjacentTerritories; 
}

// Territory setters
void Territory::setArmyUnits(int units) { armyUnits = units; }
void Territory::setOwner(Player* player) { owner = player; }
void Territory::setContinent(const std::string& continentName) { continent = continentName; }

// Territory operations
void Territory::addAdjacentTerritory(Territory* territory) {
    if (territory && !isAdjacent(territory)) {
        adjacentTerritories.push_back(territory);
    }
}

bool Territory::isAdjacent(const Territory* territory) const {
    return std::find(adjacentTerritories.begin(), adjacentTerritories.end(), territory) 
           != adjacentTerritories.end();
}

// Continent class implementation

Continent::Continent(const std::string& name) : name(name) {}

Continent::Continent(const Continent& other) : name(other.name) {
    for (Territory* territory : other.territories) {
        territories.push_back(territory);
    }
}

Continent::~Continent() {
    // Territories are managed by Map, so we don't delete them here
    territories.clear();
}

Continent& Continent::operator=(const Continent& other) {
    if (this != &other) {
        name = other.name;
        territories.clear();
        for (Territory* territory : other.territories) {
            territories.push_back(territory);
        }
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Continent& continent) {
    os << "Continent: " << continent.name << " (Territories: " << continent.territories.size() << ")";
    return os;
}

// Continent getters
std::string Continent::getName() const { return name; }
const std::vector<Territory*>& Continent::getTerritories() const { return territories; }

// Continent operations
void Continent::addTerritory(Territory* territory) {
    if (territory && !containsTerritory(territory)) {
        territories.push_back(territory);
    }
}

bool Continent::containsTerritory(const Territory* territory) const {
    return std::find(territories.begin(), territories.end(), territory) != territories.end();
}

bool Continent::isConnectedSubgraph() const {
    if (territories.empty()) return true;
    
    std::vector<bool> visited(territories.size(), false);
    std::stack<Territory*> stack;
    
    // Start with first territory
    stack.push(territories[0]);
    int visitedCount = 0;
    
    while (!stack.empty()) {
        Territory* current = stack.top();
        stack.pop();
        
        // Find index of current territory
        auto it = std::find(territories.begin(), territories.end(), current);
        if (it != territories.end()) {
            int index = std::distance(territories.begin(), it);
            if (!visited[index]) {
                visited[index] = true;
                visitedCount++;
                
                // Add adjacent territories that are in the same continent
                for (Territory* adj : current->getAdjacentTerritories()) {
                    if (containsTerritory(adj)) {
                        stack.push(adj);
                    }
                }
            }
        }
    }
    
    return visitedCount == territories.size();
}

// Map class implementation

Map::Map() {}

Map::Map(const Map& other) {
    // Deep copy territories
    for (Territory* territory : other.territories) {
        territories.push_back(new Territory(*territory));
    }
    
    // Deep copy continents
    for (Continent* continent : other.continents) {
        continents.push_back(new Continent(*continent));
    }
}

Map::~Map() {
    // Clean up all territories
    for (Territory* territory : territories) {
        delete territory;
    }
    territories.clear();
    
    // Clean up all continents
    for (Continent* continent : continents) {
        delete continent;
    }
    continents.clear();
}

Map& Map::operator=(const Map& other) {
    if (this != &other) {
        // Clean up existing data
        for (Territory* territory : territories) {
            delete territory;
        }
        territories.clear();
        
        for (Continent* continent : continents) {
            delete continent;
        }
        continents.clear();
        
        // Deep copy territories
        for (Territory* territory : other.territories) {
            territories.push_back(new Territory(*territory));
        }
        
        // Deep copy continents
        for (Continent* continent : other.continents) {
            continents.push_back(new Continent(*continent));
        }
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Map& map) {
    os << "Map with " << map.territories.size() << " territories and " 
       << map.continents.size() << " continents";
    return os;
}

// Map getters
const std::vector<Territory*>& Map::getTerritories() const { return territories; }
const std::vector<Continent*>& Map::getContinents() const { return continents; }

// Map operations
void Map::addTerritory(Territory* territory) {
    if (territory && std::find(territories.begin(), territories.end(), territory) == territories.end()) {
        territories.push_back(territory);
    }
}

void Map::addContinent(Continent* continent) {
    if (continent && std::find(continents.begin(), continents.end(), continent) == continents.end()) {
        continents.push_back(continent);
    }
}

Territory* Map::findTerritory(const std::string& name) const {
    for (Territory* territory : territories) {
        if (territory->getName() == name) {
            return territory;
        }
    }
    return nullptr;
}

Continent* Map::findContinent(const std::string& name) const {
    for (Continent* continent : continents) {
        if (continent->getName() == name) {
            return continent;
        }
    }
    return nullptr;
}

bool Map::validate() const {
    return isConnectedGraph() && 
           continentsAreConnectedSubgraphs() && 
           territoriesBelongToOneContinent();
}

bool Map::isConnectedGraph() const {
    if (territories.empty()) return true;
    
    std::vector<bool> visited(territories.size(), false);
    depthFirstSearch(territories[0], visited);
    
    // Check if all territories were visited
    for (bool v : visited) {
        if (!v) return false;
    }
    return true;
}

bool Map::continentsAreConnectedSubgraphs() const {
    for (Continent* continent : continents) {
        if (!continent->isConnectedSubgraph()) {
            return false;
        }
    }
    return true;
}

bool Map::territoriesBelongToOneContinent() const {
    for (Territory* territory : territories) {
        int continentCount = 0;
        for (Continent* continent : continents) {
            if (continent->containsTerritory(territory)) {
                continentCount++;
            }
        }
        if (continentCount != 1) {
            return false;
        }
    }
    return true;
}

void Map::depthFirstSearch(Territory* start, std::vector<bool>& visited) const {
    std::stack<Territory*> stack;
    stack.push(start);
    
    while (!stack.empty()) {
        Territory* current = stack.top();
        stack.pop();
        
        // Find index of current territory
        auto it = std::find(territories.begin(), territories.end(), current);
        if (it != territories.end()) {
            int index = std::distance(territories.begin(), it);
            if (!visited[index]) {
                visited[index] = true;
                
                // Add all adjacent territories
                for (Territory* adj : current->getAdjacentTerritories()) {
                    stack.push(adj);
                }
            }
        }
    }
}

// MapLoader class implementation

MapLoader::MapLoader() {}

MapLoader::~MapLoader() {}

Map* MapLoader::loadMap(const std::string& filename) {
    Map* map = new Map();
    
    if (parseMapFile(filename, map)) {
        std::cout << "Successfully loaded map: " << filename << std::endl;
        return map;
    } else {
        std::cout << "Failed to load map: " << filename << std::endl;
        delete map;
        return nullptr;
    }
}

bool MapLoader::isValidMapFile(const std::string& filename) {
    Map* testMap = new Map();
    bool isValid = parseMapFile(filename, testMap);
    delete testMap;
    return isValid;
}

bool MapLoader::parseMapFile(const std::string& filename, Map* map) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Cannot open file: " << filename << std::endl;
        return false;
    }
    
    // Simple validation - check if file has basic map structure
    std::string line;
    bool hasContinents = false;
    bool hasTerritories = false;
    
    while (std::getline(file, line)) {
        if (line == "[Continents]") hasContinents = true;
        if (line == "[Territories]") hasTerritories = true;
    }
    
    file.clear();
    file.seekg(0);
    
    if (!hasContinents || !hasTerritories) {
        std::cout << "Invalid map file structure" << std::endl;
        return false;
    }
    
    // Parse continents and territories
    return parseContinents(file, map) && parseTerritories(file, map);
}

bool MapLoader::parseContinents(std::ifstream& file, Map* map) {
    std::string line;
    
    // Find continents section
    while (std::getline(file, line) && line != "[Continents]") {}
    
    // Parse continents
    while (std::getline(file, line) && !line.empty()) {
        std::istringstream iss(line);
        std::string continentName;
        std::getline(iss, continentName, '=');
        
        if (!continentName.empty()) {
            Continent* continent = new Continent(continentName);
            map->addContinent(continent);
        }
    }
    
    return true;
}

bool MapLoader::parseTerritories(std::ifstream& file, Map* map) {
    std::string line;
    std::vector<std::vector<std::string>> adjacencyData;
    
    // Find territories section
    while (std::getline(file, line) && line != "[Territories]") {}
    
    // Parse territories
    while (std::getline(file, line) && !line.empty()) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 3) {
            std::string territoryName = tokens[0];
            std::string continentName = tokens[2];
            
            // Find or create continent
            Continent* continent = map->findContinent(continentName);
            if (!continent) {
                continent = new Continent(continentName);
                map->addContinent(continent);
            }
            
            // Create territory
            Territory* territory = new Territory(territoryName, continentName);
            map->addTerritory(territory);
            continent->addTerritory(territory);
            
            // Store adjacency data for later processing
            adjacencyData.push_back(tokens);
        }
    }
    
    // Create adjacency relationships
    createAdjacency(map, adjacencyData);
    return true;
}

void MapLoader::createAdjacency(Map* map, const std::vector<std::vector<std::string>>& adjacencyData) {
    for (const auto& tokens : adjacencyData) {
        if (tokens.size() < 4) continue;
        
        std::string territoryName = tokens[0];
        Territory* currentTerritory = map->findTerritory(territoryName);
        
        if (!currentTerritory) continue;
        
        // Process adjacent territories (starting from index 3)
        for (size_t i = 3; i < tokens.size(); i++) {
            std::string adjName = tokens[i];
            Territory* adjacentTerritory = map->findTerritory(adjName);
            
            if (adjacentTerritory) {
                currentTerritory->addAdjacentTerritory(adjacentTerritory);
            }
        }
    }
}

// Free function implementation
void testLoadMaps() {
    std::cout << "=== Testing Map Loading ===" << std::endl;
    
    MapLoader loader;
    
    // Test with valid map files (you'll need to provide actual file paths)
    std::vector<std::string> testFiles = {
        "valid_map1.map",
        "valid_map2.map",
        "invalid_map1.map",
        "invalid_map2.map"
    };
    
    for (const auto& filename : testFiles) {
        std::cout << "\nLoading: " << filename << std::endl;
        
        if (loader.isValidMapFile(filename)) {
            Map* map = loader.loadMap(filename);
            if (map) {
                std::cout << "Map validation result: " << (map->validate() ? "VALID" : "INVALID") << std::endl;
                
                // Demonstrate validation checks
                std::cout << " - Connected graph: " << (map->isConnectedGraph() ? "YES" : "NO") << std::endl;
                std::cout << " - Connected continents: " << (map->continentsAreConnectedSubgraphs() ? "YES" : "NO") << std::endl;
                std::cout << " - One continent per territory: " << (map->territoriesBelongToOneContinent() ? "YES" : "NO") << std::endl;
                
                delete map;
            }
        } else {
            std::cout << "Invalid map file - rejected" << std::endl;
        }
    }
    
    std::cout << "\n=== Map Loading Test Complete ===" << std::endl;
}